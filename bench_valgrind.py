import os


def run():
    start = 50
    end = 1000
    step = 50
    fns = ["ijk", "jik", "jki", "kij", "ikj", "kij"]
    mm_bin = "./a.out"
    for n in range(start, end + 1, step):
        for fn in fns:
            cmd = f"valgrind --tool=cachegrind {mm_bin} -n {n} -f {fn}"
            output = os.popen(cmd).read()
            print(output, end="")


if __name__ == "__main__":
    run()
