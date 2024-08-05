import { Embed, Webhook } from "jsr:@teever/ez-hook";

const webhookUrl = Deno.env.get("MESSAGE_WEBHOOK");
if (!webhookUrl) {
    throw new Error("Missing MESSAGE_WEBHOOK");
}

const files = new Map<string, string>();
for (const arg of Deno.args) {
    const [key, file] = arg.split("::");
    files.set(key, file);
}

console.log(files);

await new Webhook(webhookUrl)
    .setUsername("Website")
    .addEmbed(
        new Embed()
            .setTitle("New release")
            .addField({ name: "cdrom", value: files.get("cdrom")! })
            .addField({
                name: "cdrom (no packages)",
                value: files.get("cdromMinimal")!,
            })
            .addField({ name: "libs", value: files.get("libs")! })
            .addField({ name: "screenshot", value: files.get("screenshot")! }),
    ).send();
