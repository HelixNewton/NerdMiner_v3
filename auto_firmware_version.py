import subprocess

Import("env")

def get_firmware_specifier_build_flag():
    try:
        ret = subprocess.run(["git", "describe"], stdout=subprocess.PIPE, text=True) #Uses only annotated tags
        #ret = subprocess.run(["git", "describe", "--tags"], stdout=subprocess.PIPE, text=True) #Uses any tags
        build_version = ret.stdout.strip()
    except OSError:
        build_version = ""
    # fix unwanted and verbose tags
    build_version = build_version.replace('Release-', '').replace('Release', '')
    if not build_version:
        # Not a git checkout (or no tags): let version.h fall back to CURRENT_VERSION
        print("Firmware Revision: unknown (git describe failed)")
        return None
    build_flag = "-D AUTO_VERSION=\\\"" + build_version + "\\\""
    print ("Firmware Revision: " + build_version)
    return (build_flag)

flag = get_firmware_specifier_build_flag()
if flag:
    env.Append(BUILD_FLAGS=[flag])

# Board identity for Fleet OTA. ESP.getChipModel() cannot tell an S3 DevKit from
# an S3 AMOLED, so "Update all" would happily push one board's image to the
# other: same chip, different displays and pins. The PlatformIO env name is the
# only thing that uniquely names a build target.
env.Append(BUILD_FLAGS=["-D NM_BOARD_ID=\\\"" + env["PIOENV"] + "\\\""])
