# JUCE + AWS App Integration Example

## Supported Platforms

- macOS (Universal 32-bit/64-bit), 10.9 and up.
- Windows (32-bit), 7 and up.
- Windows (x64), 7 and up.

---

## Get Started - After Cloning & Forking

### Typical Update

Run the following git commands from the top-most level of the repository:

```bash
# Download the latest code from the submodules, assuming there are changes:
git submodule update --init --recursive
```

### Purging Submodules to Forcibly Update

If you're running into troubles updating your submodules, try cleaning up everything and removing the local submodule files first before re-updating them.

You may use the quick and dirty script below to facilitate this process:

```bash
git clean -dfx
git reset --hard
git fetch --all
git rebase upstream/main
git submodule update --init --recursive
```

---

## How to Build the Application

* Open, build and run the `Projucer` project.
  * Found under `submodules/JUCE/extras/Projucer/Builds`.
* Open the `JUCEAWSExample.jucer` file, hit save, and open the appropriate project for your IDE.

---

## Submodule Dependencies

### Third Party

- [JUCE](https://github.com/juce-framework/JUCE)
- [OpenSSL](https://github.com/openssl/openssl.git)
- [AWS SDK C++](https://github.com/aws/aws-sdk-cpp.git)
