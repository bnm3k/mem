import os
import sys
import hashlib
import time
import datetime as dt

import duckdb


def init_db(conn):
    conn.sql(
        """
    create sequence if not exists seq_bench_id start 1;

    create table if not exists benchmark(
        id INTEGER PRIMARY KEY DEFAULT nextval('seq_bench_id'),
        description VARCHAR,
        hash BLOB NOT NULL UNIQUE,
        ts_added TIMESTAMP NOT NULL,
        cycles_frequency_GHz DOUBLE NOT NULL,
    );

    create table if not exists results(
        bench_id INTEGER NOT NULL REFERENCES benchmark(id),
        N INTEGER NOT NULL,
        jki DOUBLE NOT NULL,
        kji DOUBLE NOT NULL,
        jik DOUBLE NOT NULL,
        ijk DOUBLE NOT NULL,
        kij DOUBLE NOT NULL,
        ikj DOUBLE NOT NULL
    );
    """
    )


def main():
    print("adding results")
    results_dir = os.path.dirname(os.path.realpath(__file__))
    results_csv = os.path.join(results_dir, "results.csv")
    results_db = os.path.join(results_dir, "results.db")

    description = None
    skip_add_description = True
    if skip_add_description == False:
        got = input("Enter benchmark description: ").strip()
        if got != "":
            description = got

    print("results_dir:", results_dir)
    print("results_csv:", results_csv)
    print("results_db:", results_db)
    print(f"description: '{description}'")

    results_db = ":memory:"
    conn = duckdb.connect(results_db)
    init_db(conn)
    conn.sql(
        f"create temporary table raw_results as select * from '{results_csv}'"
    )
    conn.sql("select * from raw_results").show()

    # get hash
    md5_hash = None
    with open(results_csv, "rb") as f:
        data = f.read()
        md5_hash = hashlib.md5(data).digest()

    # get timestamp
    timestamp = dt.datetime.now()

    # get frequency
    frequency = (
        conn.sql("select frequency_GHz from raw_results limit 1").fetchone()
        or (2600,)
    )[0]

    # insert benchmark metadata
    bench_id = None
    try:
        bench_id = (
            conn.execute(
                """insert into benchmark
                 (description, hash, ts_added, cycles_frequency_GHz) values
                 (?,?,?,?) returning id;
                 """,
                [description, md5_hash, timestamp, frequency],
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
        insert into results by name
        select * exclude frequency_GHz
        from raw_results,
        (select ? as bench_id)
        """,
        [bench_id],
    )

    conn.sql("select * from results").show()


if __name__ == "__main__":
    main()
