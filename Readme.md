📘 UltraFast File Transfer – README
🔹 কী এই সফটওয়্যার?

UltraFast হলো একটি secure file transfer system, যেখানে দুইটি আলাদা অ্যাপ ব্যবহার হয়:

UltraFastReceiver.exe → ফাইল receive করার জন্য

UltraFastSender.exe → ফাইল send করার জন্য

এই অ্যাপগুলো Windows 10 / Windows 11-এর জন্য তৈরি।

🔹 System Requirements

Windows 10 / Windows 11 (64-bit)

Internet / Local Network (LAN / Wi-Fi)

Administrator permission (install করার সময় একবার)

🔹 কীভাবে Install করবেন (One-Click)
📌 Sender Install

UltraFastSenderSetup_signed.exe ডাবল-ক্লিক করুন

Windows জিজ্ঞেস করলে Yes (UAC) চাপুন

Install শেষ হলে:

App নিজে থেকেই চালু হবে

Desktop-এ UltraFast Sender shortcut তৈরি হবে

📌 Receiver Install

UltraFastReceiverSetup_signed.exe ডাবল-ক্লিক করুন

Yes (UAC) দিন

Install শেষ হলে:

App auto-run হবে

Desktop-এ UltraFast Receiver shortcut থাকবে

🔹 Firewall নিয়ে গুরুত্বপূর্ণ তথ্য

❌ Firewall OFF করতে হবে না

✔️ Installer নিজে থেকেই Windows Defender Firewall inbound rule add করে

Public / Private network-এ অ্যাপ ঠিকভাবে কাজ করবে

👉 এটা Windows-approved এবং নিরাপদ পদ্ধতি।

🔹 কীভাবে ব্যবহার করবেন
▶️ Receiver Side

UltraFast Receiver চালু করুন

Receiver চালু থাকলেই Sender থেকে ফাইল পাঠানো যাবে

▶️ Sender Side

UltraFast Sender চালু করুন

Receiver-এর IP / Address দিন

File select করে Send করুন

🔹 Security Information

সব EXE digitally signed

কোনো virus / malware নেই

.pfx certificate কখনো user-এর PC-তে যায় না

Firewall rule user-এর অনুমতি (UAC) নিয়ে যোগ হয়

⚠️ প্রথমবার run করার সময় Windows SmartScreen এ
“More info → Run anyway” দেখাতে পারে
(কারণ certificate self-signed)

🔹 Uninstall

Control Panel → Programs → Uninstall a program

UltraFast Sender / Receiver নির্বাচন করুন

Uninstall করুন

👉 Uninstall করলে:

Program remove হবে

Firewall rule-ও automatically remove হবে

🔹 Troubleshooting

Problem: App run হচ্ছে না
✔️ নিশ্চিত করুন Receiver আগে চালু আছে

Problem: Network-এ connect হচ্ছে না
✔️ Same network / correct IP ব্যবহার করুন

Problem: Firewall issue
✔️ Installer ব্যবহার করলে আলাদা কিছু করার দরকার নেই

🔹 Important Notes

Installer একবার run করলেই যথেষ্ট

পরের বার Desktop shortcut থেকেই app চালাবেন

Installer ফাইল delete হলেও সমস্যা নেই

🔹 Contact / Developer Note

এই সফটওয়্যারটি internal / controlled use-এর জন্য ডিজাইন করা।
Public release করলে official code-signing certificate ব্যবহার করা যেতে পারে।

✅ সংক্ষেপে

✔️ One-click install
✔️ Firewall-safe
✔️ Signed & secure
✔️ Easy file transfer