import os, time, subprocess

algorithmVariants = ["vectoroptimized", "dllist", "map", "pqueue", "pqlimitedsz"]


def buildProjects():
    os.system("chcp 65001")

    for variant in algorithmVariants:
        vcxproj_path = os.path.join(f"strongin-{variant}/strongin-{variant}.vcxproj")
        print(f"Building {variant}")

        command = f"\"D:\VisualStudio\Common7\Tools\VsDevCmd.bat\" && msbuild strongin-{variant}/strongin-{variant}.vcxproj /p:Configuration=Release /p:Platform=x64 /t:Build /v:minimal"

        build_res = subprocess.run(
            command,
            capture_output=True,
            text=True,
            shell=True
        )

        if build_res.returncode == 0:
            print(f"Built {variant} successfully")
        else:
            print(f"Ошибка сборки {variant}: {build_res.stderr}", end="")
            os.system("cd")
            continue


def fillFiles():
    os.system("chcp 65001")

    for variant in algorithmVariants:
        command = "cd strongin-" + variant + " && "
        command += "\"x64/Release/strongin-" + variant + ".exe\""

        process = subprocess.run(command, shell=True)


buildProjects()
fillFiles()
