#include "chibi.h"
#include <stdio.h>

static int labelseq = 1;
static int brkseq;
static int contseq;
static char* funcname;

static void gen(Node* node, FILE* f);

// Pushes the given node's address to the stack.
static void gen_addr(Node* node, FILE* f) {
	switch (node->kind) {
		case ND_VAR: {
			if (node->init) gen(node->init, f);

			Var* var = node->var;
			if (var->is_local) {
				fprintf(f, "\tlea eax, [ebp-%d]\n", var->offset);
				fprintf(f, "\tpush eax\n");
			} else {
				fprintf(f, "\tpush %s\n", var->name);
			}
			return;
		}
		case ND_DEREF:
			gen(node->lhs, f);
			return;
		case ND_MEMBER:
			gen_addr(node->lhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tadd eax, %d\n", node->member->offset);
			fprintf(f, "\tpush eax\n");
			return;
		default:
			abort();
	}

	error_tok(node->tok, "not an lvalue");
}

static void gen_lval(Node* node, FILE* f) {
	if (node->ty->kind == TY_ARRAY) error_tok(node->tok, "not an lvalue");
	gen_addr(node, f);
}

static void load(Type* ty, FILE* f) {
	fprintf(f, "\tpop eax\n");

	if (ty->size == 1) {
		fprintf(f, "\tmov al, byte [eax]\n");
		fprintf(f, "\tand eax, 0xff\n");
	} else if (ty->size == 2) {
		fprintf(f, "\tmov ax, word [eax]\n");
		fprintf(f, "\tand eax, 0xffff\n");
	} else if (ty->size == 4) {
		fprintf(f, "\tmov eax, dword [eax]\n");
	} else {
		assert(ty->size == 8);
		abort();
	}

	fprintf(f, "\tpush eax\n");
}

static void store(Node* node, Type* ty, FILE* f) {
	fprintf(f, "\tpop edi\n");
	fprintf(f, "\tpop eax\n");

	if (ty->kind == TY_BOOL) {
		fprintf(f, "\txor ebx, ebx\n");
		fprintf(f, "\tcmp edi, 0\n");
		fprintf(f, "\tsetne bl\n");
		fprintf(f, "\tmov edi, ebx\n");
	}

	if (ty->size == 1) {
		fprintf(f, "\tmov ebx, edi\n");
		fprintf(f, "\tmov [eax], bl\n");
	} else if (ty->size == 2) {
		abort();
	} else if (ty->size == 4) {
		fprintf(f, "\tmov [eax], edi\n");
	} else {
        error_tok(node->tok, "Invalid size");
	}

	fprintf(f, "\tpush edi\n");
}

static void truncate(Type* ty, FILE* f) {
	fprintf(f, "\tpop eax\n");

	if (ty->kind == TY_BOOL) {
		fprintf(f, "\tcmp eax, 0\n");
		fprintf(f, "\tsetne al\n");
	}

	if (ty->size == 1) {
		fprintf(f, "\tmovsx eax, al\n");
	} else if (ty->size == 2) {
		fprintf(f, "\tmovsx eax, ax\n");
	} else if (ty->size == 4) {
	}
	fprintf(f, "\tpush eax\n");
}

static void inc(Type* ty, FILE* f) {
	fprintf(f, "\tpop eax\n");
	fprintf(f, "\tadd eax, %d\n", ty->base ? ty->base->size : 1);
	fprintf(f, "\tpush eax\n");
}

static void dec(Type* ty, FILE* f) {
	fprintf(f, "\tpop eax\n");
	fprintf(f, "\tsub eax, %d\n", ty->base ? ty->base->size : 1);
	fprintf(f, "\tpush eax\n");
}

static void gen_binary(Node* node, FILE* f) {
	fprintf(f, "\tpop edi\n");
	fprintf(f, "\tpop eax\n");

	switch (node->kind) {
		case ND_ADD:
		case ND_ADD_EQ:
			fprintf(f, "\tadd eax, edi\n");
			break;
		case ND_PTR_ADD:
		case ND_PTR_ADD_EQ:
			fprintf(f, "\timul edi, %d\n", node->ty->base->size);
			fprintf(f, "\tadd eax, edi\n");
			break;
		case ND_SUB:
		case ND_SUB_EQ:
			fprintf(f, "\tsub eax, edi\n");
			break;
		case ND_PTR_SUB:
		case ND_PTR_SUB_EQ:
			fprintf(f, "\timul edi, %d\n", node->ty->base->size);
			fprintf(f, "\tsub eax, edi\n");
			break;
		case ND_PTR_DIFF:
			fprintf(f, "\tsub eax, edi\n");
			// printf("\tcqo\n");
			fprintf(f, "\tmov edi, %d\n", node->lhs->ty->base->size);
			fprintf(f, "\txor edx, edx\n");
			fprintf(f, "\tidiv edi\n");
			break;
		case ND_MUL:
		case ND_MUL_EQ:
			fprintf(f, "\timul eax, edi\n");
			break;
		case ND_DIV:
		case ND_DIV_EQ:
			// printf("\tcqo\n");
			fprintf(f, "\txor edx, edx\n");
			fprintf(f, "\tidiv edi\n");
			break;
        case ND_MOD:
		case ND_MOD_EQ:
			// printf("\tcqo\n");
			fprintf(f, "\txor edx, edx\n");
			fprintf(f, "\tidiv edi\n");
            fprintf(f, "\tmov eax, edx\n");
			break;
		case ND_BITAND:
		case ND_BITAND_EQ:
			fprintf(f, "\tand eax, edi\n");
			break;
		case ND_BITOR:
		case ND_BITOR_EQ:
			fprintf(f, "\tor eax, edi\n");
			break;
		case ND_BITXOR:
		case ND_BITXOR_EQ:
			fprintf(f, "\txor eax, edi\n");
			break;
		case ND_SHL:
		case ND_SHL_EQ:
			fprintf(f, "\tmov ecx, edi\n");
			fprintf(f, "\tshl eax, cl\n");
			break;
		case ND_SHR:
		case ND_SHR_EQ:
			fprintf(f, "\tmov ecx, edi\n");
			fprintf(f, "\tsar eax, cl\n");
			break;
		case ND_EQ:
			fprintf(f, "\tcmp eax, edi\n");
			fprintf(f, "\tsete al\n");
			fprintf(f, "\tand eax, 0xff\n");
			break;
		case ND_NE:
			fprintf(f, "\tcmp eax, edi\n");
			fprintf(f, "\tsetne al\n");
			fprintf(f, "\tand eax, 0xff\n");
			break;
		case ND_LT:
			fprintf(f, "\tcmp eax, edi\n");
			fprintf(f, "\tsetl al\n");
			fprintf(f, "\tand eax, 0xff\n");
			break;
		case ND_LE:
			fprintf(f, "\tcmp eax, edi\n");
			fprintf(f, "\tsetle al\n");
			fprintf(f, "\tand eax, 0xff\n");
			break;
		default:
            error_tok(node->tok, "What?");
	}

	fprintf(f, "\tpush eax\n");
}

int push_args_reverse(Node* arg, FILE* f) {
	if (!arg) {
		return 0;
	} else {
		int nargs = push_args_reverse(arg->next, f);
		gen(arg, f);
		return nargs + 1;
	}
}

// Generate code for a given node.
static void gen(Node* node, FILE* f) {
	switch (node->kind) {
		case ND_NULL:
			return;
		case ND_NUM:
			if (node->val == (int)node->val) {
				fprintf(f, "\tpush %d\n", node->val);
			} else {
				fprintf(f, "\tmov eax, %d\n", (unsigned int)node->val);
				fprintf(f, "\tpush eax\n");
			}
			return;
		case ND_EXPR_STMT:
			gen(node->lhs, f);
			fprintf(f, "\tadd esp, 4\n");
			return;
		case ND_VAR:
			if (node->init) gen(node->init, f);
			gen_addr(node, f);
			if (node->ty->kind != TY_ARRAY) load(node->ty, f);
			return;
		case ND_MEMBER:
			gen_addr(node, f);
			if (node->ty->kind != TY_ARRAY) load(node->ty, f);
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs, f);
			gen(node->rhs, f);
			store(node, node->ty, f);
			return;
		case ND_TERNARY: {
			int seq = labelseq++;
			gen(node->cond, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tje\t.L.else.%d\n", seq);
			gen(node->then, f);
			fprintf(f, "\tjmp .L.end.%d\n", seq);
			fprintf(f, ".L.else.%d:\n", seq);
			gen(node->els, f);
			fprintf(f, ".L.end.%d:\n", seq);
			return;
		}
		case ND_PRE_INC:
			gen_lval(node->lhs, f);
			fprintf(f, "\tpush dword [esp]\n");
			load(node->ty, f);
			inc(node->ty, f);
			store(node, node->ty, f);
			return;
		case ND_PRE_DEC:
			gen_lval(node->lhs, f);
			fprintf(f, "\tpush dword [esp]\n");
			load(node->ty, f);
			dec(node->ty, f);
			store(node, node->ty, f);
			return;
		case ND_POST_INC:
			gen_lval(node->lhs, f);
			fprintf(f, "\tpush dword [esp]\n");
			load(node->ty, f);
			inc(node->ty, f);
			store(node, node->ty, f);
			dec(node->ty, f);
			return;
		case ND_POST_DEC:
			gen_lval(node->lhs, f);
			fprintf(f, "\tpush dword [esp]\n");
			load(node->ty, f);
			dec(node->ty, f);
			store(node, node->ty, f);
			inc(node->ty, f);
			return;
		case ND_ADD_EQ:
		case ND_PTR_ADD_EQ:
		case ND_SUB_EQ:
		case ND_PTR_SUB_EQ:
		case ND_MUL_EQ:
		case ND_DIV_EQ:
        case ND_MOD_EQ:
		case ND_SHL_EQ:
		case ND_SHR_EQ:
		case ND_BITAND_EQ:
		case ND_BITOR_EQ:
		case ND_BITXOR_EQ:
			gen_lval(node->lhs, f);
			fprintf(f, "\tpush dword [esp]\n");
			load(node->lhs->ty, f);
			gen(node->rhs, f);
			gen_binary(node, f);
			store(node, node->ty, f);
			return;
		case ND_COMMA:
			gen(node->lhs, f);
			gen(node->rhs, f);
			return;
		case ND_ADDR:
			gen_addr(node->lhs, f);
			return;
		case ND_DEREF:
			gen(node->lhs, f);
			if (node->ty->kind != TY_ARRAY) load(node->ty, f);
			return;
		case ND_NOT:
			gen(node->lhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tsete al\n");
			fprintf(f, "\tand eax, 0xff\n");
			fprintf(f, "\tpush eax\n");
			return;
		case ND_BITNOT:
			gen(node->lhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tnot eax\n");
			fprintf(f, "\tpush eax\n");
			return;
		case ND_LOGAND: {
			int seq = labelseq++;
			gen(node->lhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tje\t.L.false.%d\n", seq);
			gen(node->rhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tje\t.L.false.%d\n", seq);
			fprintf(f, "\tpush 1\n");
			fprintf(f, "\tjmp .L.end.%d\n", seq);
			fprintf(f, ".L.false.%d:\n", seq);
			fprintf(f, "\tpush 0\n");
			fprintf(f, ".L.end.%d:\n", seq);
			return;
		}
		case ND_LOGOR: {
			int seq = labelseq++;
			gen(node->lhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tjne .L.true.%d\n", seq);
			gen(node->rhs, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tjne .L.true.%d\n", seq);
			fprintf(f, "\tpush 0\n");
			fprintf(f, "\tjmp .L.end.%d\n", seq);
			fprintf(f, ".L.true.%d:\n", seq);
			fprintf(f, "\tpush 1\n");
			fprintf(f, ".L.end.%d:\n", seq);
			return;
		}
		case ND_IF: {
			int seq = labelseq++;
			if (node->els) {
				gen(node->cond, f);
				fprintf(f, "\tpop eax\n");
				fprintf(f, "\tcmp eax, 0\n");
				fprintf(f, "\tje\t.L.else.%d\n", seq);
				gen(node->then, f);
				fprintf(f, "\tjmp .L.end.%d\n", seq);
				fprintf(f, ".L.else.%d:\n", seq);
				gen(node->els, f);
				fprintf(f, ".L.end.%d:\n", seq);
			} else {
				gen(node->cond, f);
				fprintf(f, "\tpop eax\n");
				fprintf(f, "\tcmp eax, 0\n");
				fprintf(f, "\tje\t.L.end.%d\n", seq);
				gen(node->then, f);
				fprintf(f, ".L.end.%d:\n", seq);
			}
			return;
		}
		case ND_WHILE: {
			int seq = labelseq++;
			int brk = brkseq;
			int cont = contseq;
			brkseq = contseq = seq;

			fprintf(f, ".L.continue.%d:\n", seq);
			gen(node->cond, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tje\t.L.break.%d\n", seq);
			gen(node->then, f);
			fprintf(f, "\tjmp .L.continue.%d\n", seq);
			fprintf(f, ".L.break.%d:\n", seq);

			brkseq = brk;
			contseq = cont;
			return;
		}
		case ND_FOR: {
			int seq = labelseq++;
			int brk = brkseq;
			int cont = contseq;
			brkseq = contseq = seq;

			if (node->init) gen(node->init, f);
			fprintf(f, ".L.begin.%d:\n", seq);
			if (node->cond) {
				gen(node->cond, f);
				fprintf(f, "\tpop eax\n");
				fprintf(f, "\tcmp eax, 0\n");
				fprintf(f, "\tje\t.L.break.%d\n", seq);
			}
			gen(node->then, f);
			fprintf(f, ".L.continue.%d:\n", seq);
			if (node->inc) gen(node->inc, f);
			fprintf(f, "\tjmp .L.begin.%d\n", seq);
			fprintf(f, ".L.break.%d:\n", seq);

			brkseq = brk;
			contseq = cont;
			return;
		}
		case ND_DO: {
			int seq = labelseq++;
			int brk = brkseq;
			int cont = contseq;
			brkseq = contseq = seq;

			fprintf(f, ".L.begin.%d:\n", seq);
			gen(node->then, f);
			fprintf(f, ".L.continue.%d:\n", seq);
			gen(node->cond, f);
			fprintf(f, "\tpop eax\n");
			fprintf(f, "\tcmp eax, 0\n");
			fprintf(f, "\tjne .L.begin.%d\n", seq);
			fprintf(f, ".L.break.%d:\n", seq);

			brkseq = brk;
			contseq = cont;
			return;
		}
		case ND_SWITCH: {
			int seq = labelseq++;
			int brk = brkseq;
			brkseq = seq;
			node->case_label = seq;

			gen(node->cond, f);
			fprintf(f, "\tpop eax\n");

			for (Node* n = node->case_next; n; n = n->case_next) {
				n->case_label = labelseq++;
				n->case_end_label = seq;
				fprintf(f, "\tcmp eax, %d\n", n->val);
				fprintf(f, "\tje .L.case.%d\n", n->case_label);
			}

			if (node->default_case) {
				int i = labelseq++;
				node->default_case->case_end_label = seq;
				node->default_case->case_label = i;
				fprintf(f, "\tjmp .L.case.%d\n", i);
			}

			fprintf(f, "\tjmp .L.break.%d\n", seq);
			gen(node->then, f);
			fprintf(f, ".L.break.%d:\n", seq);

			brkseq = brk;
			return;
		}
		case ND_CASE:
			fprintf(f, ".L.case.%d:\n", node->case_label);
			gen(node->lhs, f);
			return;
		case ND_BLOCK:
		case ND_STMT_EXPR:
			for (Node* n = node->body; n; n = n->next) gen(n, f);
			return;
		case ND_BREAK:
			if (brkseq == 0) error_tok(node->tok, "stray break");
			fprintf(f, "\tjmp .L.break.%d\n", brkseq);
			return;
		case ND_CONTINUE:
			if (contseq == 0) error_tok(node->tok, "stray continue");
			fprintf(f, "\tjmp .L.continue.%d\n", contseq);
			return;
		case ND_GOTO:
			fprintf(f, "\tjmp .L.label.%s.%s\n", funcname, node->label_name);
			return;
		case ND_LABEL:
			fprintf(f, ".L.label.%s.%s:\n", funcname, node->label_name);
			gen(node->lhs, f);
			return;
		case ND_FUNCALL: {
			if (!strcmp(node->funcname, "__builtin_va_start")) {
				abort();
				return;
			}

			int nargs = push_args_reverse(node->args, f);
			// for (Node *arg = node->args; arg; arg = arg->next) {
			//\t gen(arg);
			//\t nargs++;
			// }

			// We need to align RSP to a 16 byte boundary before
			// calling a function because it is an ABI requirement.
			// int seq = labelseq++;
			fprintf(f, "\tcall %s\n", node->funcname);
			fprintf(f, "\tadd esp, %d\n", nargs * 4);
			if (node->ty->kind == TY_BOOL) fprintf(f, "\tand eax, 0xff\n");
			fprintf(f, "\tpush eax\n");
			return;
		}
		case ND_RETURN:
			if (node->lhs) {
				gen(node->lhs, f);
				fprintf(f, "\tpop eax\n");
			}
			fprintf(f, "\tjmp .L.return.%s\n", funcname);
			return;
		case ND_CAST:
			gen(node->lhs, f);
			truncate(node->ty, f);
			return;
		default:
            break;
			// error_tok(node->tok, "What %d?", node->kind);
	}

	gen(node->lhs, f);
	gen(node->rhs, f);
	gen_binary(node, f);
}

static void emit_data(Program* prog, FILE* f, bool raw) {
	for (VarList* vl = prog->globals; vl; vl = vl->next)
		if (!vl->var->is_static) {
			if (vl->var->is_extern) {
                if (!raw) {
				    fprintf(f, "extrn %s\n", vl->var->name);
                }
            } else {
                if (!raw) {
				    fprintf(f, "public %s\n", vl->var->name);
                }
            }
		}

    if (raw) {
        fprintf(f, "segment readable writeable\n");
        fprintf(f, "_bss_start:\n");
    } else {
    	fprintf(f, "section '.bss'\n");
    }

	for (VarList* vl = prog->globals; vl; vl = vl->next) {
		Var* var = vl->var;
		if (var->is_extern) continue;
		if (var->initializer) continue;

		fprintf(f, "align %d\n", var->ty->align);
		fprintf(f, "%s:\n", var->name);
		fprintf(f, "\tdb %d dup (0)\n", var->ty->size);
	}

    if (raw) {
        fprintf(f, "_bss_end:\n");
    } else {
    	fprintf(f, "section '.data'\n");
    }

	for (VarList* vl = prog->globals; vl; vl = vl->next) {
		Var* var = vl->var;
		if (var->is_extern) continue;
		if (!var->initializer) continue;

		fprintf(f, "align %d\n", var->ty->align);
		fprintf(f, "%s:\n", var->name);

		for (Initializer* init = var->initializer; init; init = init->next) {
			if (init->label)
				fprintf(f, "\tdd %s+%d\n", init->label, init->addend);
			else if (init->sz == 1)
				fprintf(f, "\tdb %d\n", init->val);
			else if (init->sz == 2)
				fprintf(f, "\tdw %d\n", init->val);
			else if (init->sz == 4)
				fprintf(f, "\tdd %d\n", init->val);
			else if (init->sz == 8)
				fprintf(f, "\tdq %d\n", init->val);
			else
				error("?");
		}
	}
}

static void load_arg(Var* var, int idx, int size_so_far, FILE* f) {
	int sz = var->ty->size;
	if (sz == 1) {
		fprintf(f, "\tmov al, [ebp+%d] ; %s\n", (idx * 4) + 8, var->name);
		fprintf(f, "\tmov [ebp-%d], al\n", var->offset);
	} else if (sz == 2) {
		fprintf(f, "\tmov ax, [ebp+%d] ; %s\n", (idx * 4) + 8, var->name);
		fprintf(f, "\tmov [ebp-%d], ax\n", var->offset);
	} else if (sz == 4) {
		fprintf(f, "\tmov eax, [ebp+%d] ; %s\n", (idx * 4) + 8, var->name);
		fprintf(f, "\tmov [ebp-%d], eax\n", var->offset);
	} else {
		assert(sz == 8);
		// printf("\tmov [rbp-%d], %s\n", var->offset, argreg8[idx]);
		abort();
	}
}

static void emit_text(Program* prog, FILE* f, bool raw) {
    if (!raw) {
	    fprintf(f, "section '.text'\n");
    } else {
        fprintf(f, "align 0x1000\n");
        fprintf(f, "segment readable executable\n");
    }

    fprintf(f, 
        "_syscall:\n"
        "\tpush ebp\n"
        "\tmov ebp, esp\n"

        "\tlea eax, [ebp+8] ; regs\n"
        "\tmov esp, dword [eax]\n"

        "\tmov eax, dword [esp + 0]\n"
        "\tmov ebx, dword [esp + 4]\n"
        "\tmov ecx, dword [esp + 8]\n"
        "\tmov edx, dword [esp + 12]\n"
        "\tmov esi, dword [esp + 16]\n"
        "\tmov edi, dword [esp + 20]\n"

    #if 1
        "\tint 0x30\n"
    #else
        "\tint 0x80\n"
    #endif

        "\tmov dword [esp + 0], eax\n"
        "\tmov dword [esp + 4], ebx\n"
        "\tmov dword [esp + 8], ecx\n"
        "\tmov dword [esp + 12], edx\n"
        "\tmov dword [esp + 16], esi\n"
        "\tmov dword [esp + 20], edi\n"

        "\tmov esp, ebp\n"
        "\tpop ebp\n"
        "\tret\n"
    );

	for (Function* fn = prog->fns; fn; fn = fn->next) {
		if (fn->is_prototype) {
            if (!raw) {
			    fprintf(f, "extrn %s\n", fn->name);
            }
			continue;
		}
		if (!fn->is_static && !raw) fprintf(f, "public %s\n", fn->name);
		fprintf(f, "%s:\n", fn->name);
		funcname = fn->name;

		// Prologue
		fprintf(f, "\tpush ebp\n");
		fprintf(f, "\tmov ebp, esp\n");
		fprintf(f, "\tsub esp, %d\n", fn->stack_size);

		// Save arg registers if function is variadic
		if (fn->has_varargs) {
			abort();
		}

		// Push arguments to the stack
		int i = 0;
		int size_so_far = 0;
		for (VarList* vl = fn->params; vl; vl = vl->next) {
			load_arg(vl->var, i++, size_so_far, f);
			size_so_far += vl->var->ty->size;
		}
		// int i = load_args_reverse(fn->params, 0);

		// Emit code
		for (Node* node = fn->node; node; node = node->next) gen(node, f);

		// Epilogue
		fprintf(f, ".L.return.%s:\n", funcname);
		fprintf(f, "\tmov esp, ebp\n");
		fprintf(f, "\tpop ebp\n");
		fprintf(f, "\tret\n");
	}
}

void codegen(Program* prog, FILE* f, bool raw) {
    if (raw) {
	    fprintf(f, "format ELF executable at 0xA0000000\n");
        fprintf(f, "entry _start\n");
    } else {
	    fprintf(f, "format ELF\n");
    }
	emit_data(prog, f, raw);
	emit_text(prog, f, raw);
}