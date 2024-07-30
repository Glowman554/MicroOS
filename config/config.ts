interface ConfigItem {
    name: string;
    value: boolean | number | string | ConfigMenu;
}

interface ConfigMenu {
    title: string;
    items: ConfigItem[];
}

interface ConfigFile {
    file: string;
    configuration: ConfigMenu;
}

interface Syscall {
    name: string;
    arguments: string[];
}

interface SyscallsList {
    syscalls: Syscall[];
}

interface Preset {
    [key: string]: boolean | number | string;
}

async function tryAgain(configMenu: ConfigMenu) {
    console.log("\nInvalid option. Please try again.\n");
    await configurePragmaOnce(configMenu);
}

async function configurePragmaOnce(configMenu: ConfigMenu): Promise<void> {
    console.log(`${configMenu.title}:`);
    for (let i = 0; i < configMenu.items.length; i++) {
        const item = configMenu.items[i];

        if (typeof (item.value) == "boolean") {
            console.log(
                `${i + 1}. ${item.name} [${
                    item.value ? "Enabled" : "Disabled"
                }]`,
            );
        } else if (typeof (item.value) == "object") {
            console.log(`${i + 1}. ${item.name}`);
        } else {
            console.log(`${i + 1}. ${item.name} [${item.value}]`);
        }
    }

    const option = prompt("Select an option (or press Enter to finish):");
    if (option == null || option == "") {
        return;
    }

    const selectedOption = parseInt(option, 10);

    if (
        isNaN(selectedOption) || selectedOption < 1 ||
        selectedOption > configMenu.items.length
    ) {
        await tryAgain(configMenu);
        return;
    }

    const selectedItem = configMenu.items[selectedOption - 1];
    if (typeof (selectedItem.value) == "boolean") {
        selectedItem.value = !selectedItem.value;
    } else if (typeof (selectedItem.value) == "number") {
        const newValue = prompt(`New Value for ${selectedItem.name}:`);
        if (newValue == null) {
            await tryAgain(configMenu);
            return;
        }
        const newValueParsed = parseInt(newValue, 10);
        if (isNaN(newValueParsed)) {
            await tryAgain(configMenu);
            return;
        }
        selectedItem.value = newValueParsed;
    } else if (typeof (selectedItem.value) == "string") {
        const newValue = prompt(`New Value for ${selectedItem.name}:`);
        if (newValue == null) {
            await tryAgain(configMenu);
            return;
        }
        selectedItem.value = newValue;
    } else if (typeof (selectedItem.value) == "object") {
        await configurePragmaOnce(selectedItem.value as ConfigMenu);
    } else {
        throw new Error("?");
    }

    await configurePragmaOnce(configMenu);
}

function generateConfiguration(
    configMenu: ConfigMenu,
    preset: Preset | undefined,
): string {
    let config = `// ${configMenu.title}\n`;

    for (let i = 0; i < configMenu.items.length; i++) {
        const configItem = configMenu.items[i];
        if (preset && configItem.name in preset) {
            configItem.value = preset[configItem.name];
        }

        if (typeof (configItem.value) == "boolean") {
            if (configItem.value) {
                config += `#define ${configItem.name}\n`;
            } else {
                config += `// #define ${configItem.name}\n`;
            }
        } else if (typeof (configItem.value) == "string") {
            config += `#define ${configItem.name} "${configItem.value}"\n`;
        } else if (typeof (configItem.value) == "object") {
            config += generateConfiguration(
                configItem.value as ConfigMenu,
                preset,
            );
        } else {
            config += `#define ${configItem.name} ${configItem.value}\n`;
        }
    }

    config += `// End ${configMenu.title}\n\n`;
    return config;
}

const has = (o: any, k: any) => Object.prototype.hasOwnProperty.call(o, k);
function mergeDefault(def: any, given: any): any {
    if (!given) return def;
    for (const key in def) {
        if (!has(given, key) || given[key] === undefined) {
            given[key] = def[key];
        } else if (given[key] === Object(given[key])) {
            given[key] = mergeDefault(def[key], given[key]);
        }
    }

    return given;
}

async function main() {
    let input = undefined;
    let auto = false;
    let clean = false;
    let load = undefined;

    let idx = 0;
    while (idx < Deno.args.length) {
        if (Deno.args[idx] == "--auto") {
            auto = true;
        } else if (Deno.args[idx] == "--clean") {
            clean = true;
        } else if (Deno.args[idx] == "--load") {
            if (load == undefined && !(Deno.args.length < idx + 1)) {
                idx++;
                load = Deno.args[idx];
            } else {
                throw new Error("Too many arguments!");
            }
        } else {
            if (input == undefined) {
                input = Deno.args[idx];
            } else {
                throw new Error("Too many arguments!");
            }
        }
        idx++;
    }

    if (input == undefined) {
        throw new Error("No input file!");
    }

    const userConfigFile = input + ".user";
    const configuration = JSON.parse(
        Deno.readTextFileSync(input),
    ) as ConfigFile;

    let userConfiguration: ConfigMenu | undefined;
    try {
        if (!clean) {
            userConfiguration = JSON.parse(
                Deno.readTextFileSync(userConfigFile),
            ) as ConfigMenu;
        }
    } catch (_) {}

    let preset: Preset | undefined;
    try {
        if (load) {
            preset = JSON.parse(
                Deno.readTextFileSync(load),
            ) as Preset;
        }
    } catch (_) {}

    const configMenu: ConfigMenu = mergeDefault(
        configuration.configuration,
        userConfiguration,
    ) as ConfigMenu;

    if (configMenu.items.find((item) => item.name == "Syscalls") == undefined) {
        const syscallList = JSON.parse(
            Deno.readTextFileSync("config/syscalls.json"),
        ) as SyscallsList;

        const syscallItems: ConfigItem[] = [];
        for (const syscall of syscallList.syscalls) {
            syscallItems.push({
                name: syscall.name + "_ID",
                value: parseInt(syscall.arguments[0]),
            });
        }

        configMenu.items.push({
            name: "Syscalls",
            value: {
                title: "Syscalls",
                items: syscallItems,
            },
        });
    }

    if (!auto) {
        await configurePragmaOnce(configMenu);
    }

    console.log(`Writing ${configuration.file}...`);
    Deno.writeTextFileSync(
        configuration.file,
        "#pragma once\n\n" + generateConfiguration(configMenu, preset),
    );

    if (!clean) {
        console.log(`Writing ${userConfigFile}...`);
        Deno.writeTextFileSync(
            userConfigFile,
            JSON.stringify(configMenu, null, "\t"),
        );
    }
}

main().catch((err) => console.error(err));
