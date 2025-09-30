# hsu
An insecure `su` command for macOS, that doesn't need a password and allows login as service accounts. Keep in mind **this isn't an exploit**, and it still requires sudo to be set up.

<img width="797" height="113" alt="Screenshot 2025-09-30 at 3 44 58 PM" src="https://github.com/user-attachments/assets/20c47510-c0d1-46cf-b4fa-7bdf4e9ce7c8" />

# Setup
Clone the repo, `sh install.sh` 

Precompiled binaries can be found in the Releases page (universal, arm64, x86_64 -amd64- ). \
They must be unquarantined with `xattr -c path/to/hsu`, set ownership to root with `sudo chown root:wheel path/to/hsu` and given the setuid bit with `sudo chmod u+s path/to/hsu`
