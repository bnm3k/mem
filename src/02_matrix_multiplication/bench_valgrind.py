import subprocess
import re
import sys
import csv


def parse_output(s):

    match_nums = r"(\d{1,3}(?:,\d{3})*(?:\.\d+)?)\s+\(\s*(\d{1,3}(?:,\d{3})*(?:\.\d+)?)\s+rd\s+\+\s+(\d{1,3}(?:,\d{3})*(?:\.\d+)?)\s+wr\).*"
    s = s.strip()
    lines = s.split("\n")

    patterns = {
        "d_refs": re.compile(r"==\d+==\sD\s+refs:\s+" + match_nums),
        "d1_misses": re.compile(r"==\d+==\sD1\s+misses:\s+" + match_nums),
        "lld_misses": re.compile(r"==\d+==\sLLd\s+misses:\s+" + match_nums),
    }
    res = {}
    for line in lines:
        for k, pattern in patterns.items():
            match = pattern.match(line)
            if match is not None:
                res[k] = {
                    f[0]: int(match.group(f[1]).replace(",", ""))
                    for f in [("total", 1), ("rd", 2), ("wr", 3)]
                }
    return res


def run():
    LL_cache_size_MB = 2
    LL_cache_size_bytes = LL_cache_size_MB * (2**20)  # 2MB

    start = 50
    end = 600
    step = 50

    fns = ["ijk", "jik", "jki", "kij", "ikj", "kij"]

    mm_bin = "./a.out"

    file_path = f"./cachegrind_res_LL_{LL_cache_size_MB}.csv"
    with open(file_path, mode="w", newline="") as file:
        writer = csv.writer(file)
        for n in range(start, end + 1, step):
            for fn in fns:
                cmd = f"valgrind --LL={LL_cache_size_bytes},16,64 --tool=cachegrind {mm_bin} -n {n} -f {fn}"
                output = subprocess.run(
                    cmd, shell=True, capture_output=True, text=True
                ).stderr
                res = parse_output(output)
                for k, v in res.items():
                    writer.writerow([n, fn, k, v["total"], v["rd"], v["wr"]])


if __name__ == "__main__":
    run()
