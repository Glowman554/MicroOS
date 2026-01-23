import { Embed, Webhook } from "jsr:@teever/ez-hook";

const webhookUrl = Deno.env.get("MESSAGE_WEBHOOK");
if (!webhookUrl) {
    throw new Error("Missing MESSAGE_WEBHOOK");
}

const buildToken = Deno.env.get("MICROOS_BUILD_TOKEN");
if (!buildToken) {
    throw new Error("Missing MICROOS_BUILD_TOKEN");
}

const values = new Map<string, string>();
for (const arg of Deno.args) {
    const [key, value] = arg.split("::");
    values.set(key, value);
}

console.log(values);

await new Webhook(webhookUrl)
    .setUsername("Website")
    .addEmbed(
        new Embed()
            .setTitle(`New release (${values.get("name")})`)
            .addField({ name: "cdrom", value: values.get("cdrom")! })
            .addField({
                name: "cdrom (no packages)",
                value: values.get("cdromMinimal")!,
            })
            .addField({ name: "libs", value: values.get("libs")! })
            .addField({ name: "message", value: values.get("message")! })
            .addField({ name: "screenshot", value: values.get("screenshot")! })
            .addField({ name: "kernel", value: values.get("kernel")! })
            .addField({ name: "symbols", value: values.get("symbols")! })
            .addField({ name: "initrd", value: values.get("initrd")! })
            .setImage({ url: values.get("screenshot")! }),
    ).send();

fetch("https://toxicfox.de/api/v1/microos/build", {
    method: "POST",
    headers: {
        "Content-Type": "application/json",
        Authentication: `${buildToken}`,
    },
    body: JSON.stringify({
        preset: values.get("name"),
        kernel: values.get("kernel"),
        symbols: values.get("symbols"),
        initrd: values.get("initrd"),
    }),
}).then(async (res) => {
    console.log(`Build trigger response: ${res.status} ${await res.text()}`);
});