from prettytable import PrettyTable

algorithmVariants = ["vectoroptimized", "dllist", "map", "pqueue", "pqlimitedsz"]
file = open("testing-properties.txt")
if int(file.readlines()[2].split()[1]) <= 10:
    algorithmVariants.append("base")
funcCount = 10


def checkResults():
    variantResults = dict()
    properties = set()

    for variant in algorithmVariants:
        variantResults[variant] = []

        for funcNumber in range(1, funcCount + 1):
            variantResults[variant].append(dict())
            file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
            for i in range(4):
                line = file.readline()
                propertyName = line.split()[0][:-1]
                properties.add(propertyName)
                value = float(line.split()[1])
                variantResults[variant][funcNumber - 1][propertyName] = value

    standart = []
    for funcNumber in range(funcCount):
        standart.append(dict())
        for prop in properties:
            standart[funcNumber][prop] = variantResults[algorithmVariants[0]][funcNumber][prop]

    notOk = set()
    for variant in algorithmVariants[1:]:
        for funcNumber in range(funcCount):
            for prop in properties:
                if variantResults[variant][funcNumber][prop] != standart[funcNumber][prop]:
                    notOk.add(prop)

    for badProp in notOk:
        print(badProp + " table: ")
        headers = [""]
        for funcNumber in range(1, funcCount + 1):
            headers.append(f"Function{funcNumber}")

        table = PrettyTable(headers)

        for variant in algorithmVariants:
            row = [variant]

            for funcNumber in range(funcCount):
                row.append(variantResults[variant][funcNumber][badProp])

            table.add_row(row)
        print(table)
        print()

    if notOk:
        print("######################")
        print("######################")
        print("An error has occurred!")
        print("######################")
        print("######################")


def showAGPResult():
    variant = algorithmVariants[0]
    for funcNumber in range(1, funcCount + 1):
        print(f"    Function {funcNumber}")
        file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
        for i in range(5):
            print(file.readline()[:-1])
        print()


def showTable():
    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")

    table = PrettyTable(headers)

    for variant in algorithmVariants:
        if variant == "map":
            row = ["map + multimap"]
        elif variant == "pqueue":
            row = ["pqueue + map"]
        elif variant == "pqlimitedsz":
            row = ["pqueuelimited + map"]
        else:
            row = [variant]
        for funcNumber in range(1, funcCount + 1):

            file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")

            for i in range(4):
                file.readline()

            timeSpent = float(file.readline().split(" ")[1])
            row.append(timeSpent)
        table.add_row(row)

    print("Minimum-calculating-time table:")
    print(table)


def showTables(tableNames):
    variantResults = dict()
    properties = set()

    for variant in algorithmVariants:
        variantResults[variant] = []
        for funcNumber in range(1, funcCount + 1):
            variantResults[variant].append(dict())
            file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
            for i in range(5):
                line = file.readline()
                propertyName = line.split()[0][:-1]
                properties.add(propertyName)
                value = float(line.split()[1])
                variantResults[variant][funcNumber - 1][propertyName] = value

    for name in tableNames:
        print(name + " table: ")
        headers = [""]
        for funcNumber in range(1, funcCount + 1):
            headers.append(f"Function{funcNumber}")
        table = PrettyTable(headers)

        for variant in algorithmVariants:
            if variant == "map":
                row = ["multimap + map"]
            elif variant == "pqueue":
                row = ["pqueue + map"]
            elif variant == "pqlimitedsz":
                row = ["pqueuelimited + map"]
            else:
                row = [variant]

            for funcNumber in range(funcCount):
                row.append(variantResults[variant][funcNumber][name])
            table.add_row(row)
        print(table)
        print()


showAGPResult()
showTables(["AGP-result", "Actual-result", "Difference-in-results", "Iterations-count", "Minimum-calculating-time"])
checkResults()

