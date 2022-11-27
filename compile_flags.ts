for(let i = 0; i < Deno.args.length; i++) {
	if (Deno.args[i + 1] && !Deno.args[i + 1].startsWith("-")) {
		console.log(Deno.args[i] + Deno.args[i + 1]);
		i++;
	} else {
		console.log(Deno.args[i]);
	}
}