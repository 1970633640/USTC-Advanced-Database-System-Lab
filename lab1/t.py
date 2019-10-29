x=set()
with open("data-5w-50w-zipf.txt") as f:
    for line in f.readlines():
        x.add(line.split(',')[1])
print(len(x))