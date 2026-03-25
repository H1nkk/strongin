import os, datetime, subprocess

algorithmVariants = ["omp-map", "vectoroptimized", "dllist", "map", "pqueue", "pqlimitedsz", "omp-simple"]
file = open("testing-properties.txt")
if int(file.readlines()[2].split()[1]) <= 10:
    algorithmVariants.append("base")

def buildProjects():
    os.system("chcp 65001")

    for variant in algorithmVariants:
        vcxproj_path = os.path.join(f"strongin-{variant}/strongin-{variant}.vcxproj")
        print(f"Building {variant}")
        compiler_commands = "/openmp /O2"
        command = f"\"C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\Common7\\Tools\\VsDevCmd.bat\" && msbuild strongin-{variant}/strongin-{variant}.vcxproj /p:Configuration=Release /p:AdditionalOptions=\"{compiler_commands}\" /p:Platform=x64 /t:Build /v:minimal"

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
        print("############################")
        print(f"Running {variant}")

        exe_path = os.path.join("x64", "Release", f"strongin-{variant}.exe")
        working_dir = os.path.join(os.getcwd(), f"strongin-{variant}")

        if not os.path.exists(os.path.join(working_dir, exe_path)):
            print(f"Executable not found: {exe_path} in {working_dir}")
            continue

        process = subprocess.run(
            os.path.join(working_dir, exe_path),
            cwd=working_dir,
            shell=False
        )

        if process.returncode != 0:
            print(f"Error running {variant}, return code: {process.returncode}")
        else:
            print(f"Finished {variant}")


now = datetime.datetime.now()
print("Started at", now)

buildProjects()
fillFiles()

elapsed = datetime.datetime.now() - now
now = datetime.datetime.now()
print("Ended at", now)
print("Elasped:", elapsed.total_seconds(), "seconds")
