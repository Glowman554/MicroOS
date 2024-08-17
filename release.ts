import { Embed, Webhook } from "jsr:@teever/ez-hook";

const webhookUrl = Deno.env.get("MESSAGE_WEBHOOK");
if (!webhookUrl) {
    throw new Error("Missing MESSAGE_WEBHOOK");
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
            .setImage({ url: values.get("screenshot")! }),
    ).send();
