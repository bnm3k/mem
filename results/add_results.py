import os
import sys
import hashlib
import datetime as dt

import duckdb


def init_db(conn):
    conn.sql(
        """
    create sequence seq_bench_id start 1;

    create type BENCH_TYPE
        as ENUM('inner', 'total');

    create type UNIT_TYPE
        as ENUM('cycles', 'ns');

    create table benchmark(
        id INTEGER PRIMARY KEY DEFAULT nextval('seq_bench_id'),
        description VARCHAR,
        hash BLOB NOT NULL UNIQUE,
        ts_added TIMESTAMP NOT NULL,
        frequency_GHz DOUBLE NOT NULL,
        num_samples INTEGER NOT NULL,
        convergence INTEGER NOT NULL,
        k INTEGER NOT NULL,
        epsilon DOUBLE NOT NULL,
        bench_type BENCH_TYPE NOT NULL,
        unit UNIT_TYPE NOT NULL
    );

    create type FN_TYPE
        as ENUM('jki', 'kji', 'jik', 'ijk','kij', 'ikj');

    create table result(
        bench_id INTEGER NOT NULL REFERENCES benchmark(id),
        N INTEGER NOT NULL,
        fn FN_TYPE NOT NULL,
        value DOUBLE NOT NULL
    );
    """
    )


def main():
    print("Adding results")
    results_dir = os.path.dirname(os.path.realpath(__file__))
    results_csv = os.path.join(results_dir, "results.csv")
    results_db = os.path.join(results_dir, "results.db")
    # results_db = ":memory:"

    description = None
    skip_add_description = False
    if skip_add_description == False:
        got = input("Enter benchmark description: ").strip()
        if got != "":
            description = got

    print("results_dir:", results_dir)
    print("results_csv:", results_csv)
    print("results_db:", results_db)
    print(f"description: '{description}'")

    conn = duckdb.connect(results_db)
    try:
        init_db(conn)
    except duckdb.CatalogException:
        print("db already init")

    conn.sql(
        f"create temporary table raw_results as select * from '{results_csv}'"
    )

    # get hash
    md5_hash = None
    with open(results_csv, "rb") as f:
        data = f.read()
        md5_hash = hashlib.md5(data).digest()

    # get timestamp
    timestamp = dt.datetime.now()

    # insert benchmark metadata
    bench_id = None
    try:
        bench_metadata = conn.sql(
            """
            select frequency_GHz, num_samples, convergence,
                k, epsilon, trim(bench_type), trim(unit)
            from raw_results limit 1
            """
        ).fetchone()
        benchmark = [description, md5_hash, timestamp]
        if bench_metadata is not None:
            benchmark.extend(bench_metadata)
        else:
            raise Exception("bench_metadata is None")

        bench_id = (
            conn.execute(
                """insert into benchmark
                 (description, hash, ts_added, frequency_GHz,
                 num_samples, convergence, k, epsilon, bench_type, unit)
                 values (?,?,?,?,?,?,?,?,?,?) returning id;
                 """,
                benchmark,
            ).fetchone()
            or (None,)
        )[0]
    except duckdb.ConstraintException as exc:
        if 'Duplicate key "hash' in str(exc):
            sys.exit("Error: Attempting to re-insert results already present")
        else:
            raise exc

    conn.execute(
        f"""
        insert into result by name
        select bench_id, N, trim(fn) as fn, cycles as value
        from raw_results,
        (select ? as bench_id)
        """,
        [bench_id],
    )

    conn.sql("select * from result").show()


if __name__ == "__main__":
    main()
