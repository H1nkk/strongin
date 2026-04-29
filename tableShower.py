from prettytable import PrettyTable

# algorithmVariants = ["tbb-pqueue", "vectoroptimized", "simple", "omp-simple", "map", "omp-map", "pqueue", "omp-pqueue",  "dllist", "pqlimitedsz"]
algorithmVariants = ["tbb-pqueue", "pqueue", "omp-pqueue"]
file = open("testing-properties.txt")
if int(file.readlines()[2].split()[1]) <= 10:
    algorithmVariants.append("base")
funcCount = 10


def checkResults():
    variantResults = dict()
    properties = set()

    for variant in algorithmVariants:
        if variant[:3] == "omp":
            continue
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
        if variant[:3] == "omp":
            continue
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
            if variant[:3] == "omp":
                continue
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
    headers = [""]
    variant = algorithmVariants[0]
    properties = dict()
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")
        resultsFile = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
        for i in range(4):
            stroka = resultsFile.readline()[:-1]
            if not stroka.split(": ")[0] in properties:
                properties[stroka.split(": ")[0]] = list()
            properties[stroka.split(": ")[0]].append(float(stroka.split(": ")[1]))

    table = PrettyTable(headers)

    for prop in properties:
        row = list()
        row.append(prop)
        for val in properties[prop]:
            if prop == "Iterations-count":
                row.append(int(val))
            else:
                row.append(f"{val:.6f}")
        table.add_row(row)

    print("AGP-result table:")
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


def showOmpComparison():
    variantResults = dict()
    properties = set()

    for variant in algorithmVariants:
        variantResults[variant] = []
        for funcNumber in range(1, funcCount + 1):
            variantResults[variant].append(dict())
            file = open("strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
            for i in range(7):
                line = file.readline()
                propertyName = line.split()[0][:-1]
                properties.add(propertyName)
                value = float(line.split()[1])
                variantResults[variant][funcNumber - 1][propertyName] = value


    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")


def showOmpTables(table_name):
    variantResults = dict()
    properties = set()

    for variant in algorithmVariants:
        if variant[:3] != "omp":
            if variant == "pqueue":
                variantResults[variant] = []
                for funcNumber in range(1, funcCount + 1):
                    variantResults[variant].append(dict())
                    file = open(
                        "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                    for i in range(7):
                        line = file.readline()
                        propertyName = line.split()[0][:-1]
                        properties.add(propertyName)
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value
            elif variant == "simple":
                variantResults[variant] = []
                for funcNumber in range(1, funcCount + 1):
                    variantResults[variant].append(dict())
                    file = open(
                        "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                    for i in range(7):
                        line = file.readline()
                        propertyName = line.split()[0][:-1]
                        if propertyName != table_name:
                            continue
                        properties.add(propertyName)
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value
            else:
                continue
        else:
            variantResults[variant] = []
            for funcNumber in range(1, funcCount + 1):
                variantResults[variant].append(dict())
                file = open(
                    "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    if propertyName != table_name:
                        continue
                    properties.add(propertyName)
                    value = float(line.split()[1])
                    variantResults[variant][funcNumber - 1][propertyName] = value

    print(f"{table_name} table: ")
    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")
    table = PrettyTable(headers)

    for variant in algorithmVariants:
        if variant[:3] != "omp":
            if variant == "pqueue":
                row = ["non-parallel-AGP"]
            elif variant == "simple":  # bruteforce типа перебор
                row = ["bruteforce"]

            else:
                continue
        elif variant == "omp-simple":
            row = ["omp-bruteforce"]
        else:
            row = [variant]

        for funcNumber in range(funcCount):
            row.append(variantResults[variant][funcNumber][table_name])
        table.add_row(row)
    print(table)
    print()

def showIntervalLenghts():
    variantResults = dict()
    properties = set()
    for variant in algorithmVariants:
        if variant[:3] != "omp":
            if variant == "pqueue":
                variantResults[variant] = []
                for funcNumber in range(1, funcCount + 1):
                    variantResults[variant].append(dict())
                    file = open(
                        "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                    for i in range(7):
                        line = file.readline()
                        propertyName = line.split()[0][:-1]
                        properties.add(propertyName)
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value
            elif variant == "simple":
                variantResults[variant] = []
                for funcNumber in range(1, funcCount + 1):
                    variantResults[variant].append(dict())
                    file = open(
                        "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                    for i in range(7):
                        line = file.readline()
                        propertyName = line.split()[0][:-1]
                        if propertyName != "Left-closest-argument" and propertyName != "Right-closest-argument":
                            continue
                        properties.add(propertyName)
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value
            else:
                continue
        elif variant[:3] == "tbb":
            variantResults[variant] = []
            for funcNumber in range(1, funcCount + 1):
                variantResults[variant].append(dict())
                file = open(
                    "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    properties.add(propertyName)
                    value = float(line.split()[1])
                    variantResults[variant][funcNumber - 1][propertyName] = value
        else:
            variantResults[variant] = []
            for funcNumber in range(1, funcCount + 1):
                variantResults[variant].append(dict())
                file = open(
                    "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    if propertyName != "Left-closest-argument" and propertyName != "Right-closest-argument":
                        continue
                    properties.add(propertyName)
                    value = float(line.split()[1])
                    variantResults[variant][funcNumber - 1][propertyName] = value

    print(f"Interval borders table: ")
    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")
    table = PrettyTable(headers)

    for variant in algorithmVariants:
        if variant[:3] != "omp":
            if variant == "pqueue":
                row = ["non-parallel-AGP"]
            elif variant == "simple":  # bruteforce типа перебор
                row = ["bruteforce"]

            else:
                continue
        elif variant == "omp-simple":
            row = ["omp-bruteforce"]
        else:
            row = [variant]

        for funcNumber in range(funcCount):
            intervalLenght = variantResults[variant][funcNumber]["Right-closest-argument"] - variantResults[variant][funcNumber]["Left-closest-argument"]
            row.append(f"{intervalLenght:.6f}")
        table.add_row(row)
    print(table)
    print()


def showAccelerationTables():
    variantResults = dict()
    accelerationResults = dict()
    podhodyashieVariants = list()
    for variant in algorithmVariants:
        if variant[:3] == "omp":
            # podhodyashieVariants.append(variant)
            podhodyashieVariants.append(variant[4:])


    for variant in algorithmVariants:
        if variant in podhodyashieVariants:
            variantResults[variant] = []
            accelerationResults[variant] = []
            for funcNumber in range(1, funcCount + 1):
                variantResults[variant].append(dict())
                accelerationResults[variant].append(dict())
                file = open(
                    "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    if propertyName == "Minimum-calculating-time":
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value
        if variant == "tbb-pqueue":
            variantResults[variant] = []
            accelerationResults[variant] = []
            for funcNumber in range(1, funcCount + 1):
                variantResults[variant].append(dict())
                accelerationResults[variant].append(dict())
                file = open(
                    "strongin-" + variant + "/test-results/" + variant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    if propertyName == "Minimum-calculating-time":
                        value = float(line.split()[1])
                        variantResults[variant][funcNumber - 1][propertyName] = value

    for variant in algorithmVariants:
        if variant in podhodyashieVariants:
            if variant == "pqueue":
                tbbVariant = "tbb-" + variant
                for funcNumber in range(1, funcCount + 1):
                    file = open(
                        "strongin-" + tbbVariant + "/test-results/" + tbbVariant + "-Function" + str(
                            funcNumber) + "-res.txt")
                    for i in range(7):
                        line = file.readline()
                        propertyName = line.split()[0][:-1]
                        if propertyName == "Minimum-calculating-time":
                            value = float(line.split()[1])
                            accelRes = variantResults[variant][funcNumber - 1][propertyName] / value
                            accelerationResults[tbbVariant][funcNumber - 1][propertyName] = accelRes
                            # variantResults[tbbVariant][funcNumber - 1][propertyName] = variantResults[variant][funcNumber - 1][propertyName] / value
            ompVariant = "omp-" + variant
            for funcNumber in range(1, funcCount + 1):
                file = open(
                    "strongin-" + ompVariant + "/test-results/" + ompVariant + "-Function" + str(funcNumber) + "-res.txt")
                for i in range(7):
                    line = file.readline()
                    propertyName = line.split()[0][:-1]
                    if propertyName == "Minimum-calculating-time":
                        value = float(line.split()[1])
                        accelRes = variantResults[variant][funcNumber - 1][propertyName] / value

                        accelerationResults[variant][funcNumber - 1][propertyName] = accelRes
                        # variantResults[variant][funcNumber - 1][propertyName] /= value


    podhodyashieVariants.append("tbb-pqueue")
    print(f"Acceleration table: ")
    headers = [""]
    for funcNumber in range(1, funcCount + 1):
        headers.append(f"Function{funcNumber}")
    table = PrettyTable(headers)

    for variant in podhodyashieVariants:
        if variant[:3] != "tbb":
            row = ["omp-" + variant]
        elif variant == "simple":
            row = ["omp-bruteforce"]
        else:
            row = [variant]

        for funcNumber in range(funcCount):
            row.append(accelerationResults[variant][funcNumber]["Minimum-calculating-time"])
            #row.append(variantResults[variant][funcNumber]["Minimum-calculating-time"])
        table.add_row(row)
    print(table)
    print()

showTables(["AGP-result", "Minimum-calculating-time" ])
showAGPResult()
#checkResults()
showOmpTables("Iterations-count")
showOmpTables("AGP-result")
showIntervalLenghts()
showOmpTables("Minimum-calculating-time")
showAccelerationTables()
