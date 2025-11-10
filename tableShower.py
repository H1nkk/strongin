from prettytable import PrettyTable

algorithmVariants = ["vectoroptimized", "dllist", "map", "pqueue", "pqlimitedsz"]
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


def showAGPResult():
    variant = algorithmVariants[0]
    inaccurateVariant = algorithmVariants[4]
    for funcNumber in range(1, funcCount + 1):
        print(f"    Function {funcNumber}")
        file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
        inaccurateFile = open("strongin-" + inaccurateVariant + "/test-results/" + inaccurateVariant + "-Function" + str(funcNumber) + "-res.txt")
        for i in range(5):
            print(file.readline()[:-1], end=" ")
            if i != 1:
                print(", inaccurate: " + inaccurateFile.readline().split()[1])
            else:
                inaccurateFile.readline()
                print()

        print()


def showTable():
    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")

    table = PrettyTable(headers)

    for variant in algorithmVariants:
        row = [variant]
        for funcNumber in range(1, funcCount + 1):

            file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")

            for i in range(4):
                file.readline()

            timeSpent = float(file.readline().split(" ")[1])
            row.append(timeSpent)
        table.add_row(row)

    print(table)


checkResults()
showAGPResult()
showTable()

