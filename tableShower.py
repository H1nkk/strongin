from prettytable import PrettyTable
import os, time, subprocess


algorithmVariants = ["vectoroptimized", "dllist", "map", "pqueue"]


"""def fillFiles():
    os.system("chcp 1252")

    for variant in algorithmVariants:
        command = "cd strongin-" + variant + " && "
        command += "start ../x64/Release/strongin-" + variant + ".exe"
        pathToExe = "strongin/x64/Release/strongin-" + variant + ".exe"

        for i in range(2):  # 2 раза для прогрева
            process = subprocess.Popen("x64\\Release\\strongin-" + variant + ".exe", shell=True)
            process.wait()
            # time.sleep(3)
"""


def showAGPResult():
    variant = algorithmVariants[0]
    for funcNumber in range(1, 11):
        print(f"    Function {funcNumber}")
        file = open("strongin-" + variant + "/" + variant + "-Function" + str(funcNumber) + "-res.txt")
        for i in range(5):
            print(file.readline(), end="")
        print()


def showTable():
    headers = [""]
    for funcNumber in range(1, 11):
        headers.append(f"Function{funcNumber}")

    table = PrettyTable(headers)

    for variant in algorithmVariants:
        row = [variant]
        for funcNumber in range(1, 11):

            file = open("strongin-" + variant + "/" + variant + "-Function" + str(funcNumber) + "-res.txt")

            for i in range(4):
                file.readline()

            timeSpent = float(file.readline().split(" ")[1])
            row.append(timeSpent)
        table.add_row(row)

    print(table)


showAGPResult()
showTable()
