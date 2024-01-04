#![allow(unused_imports)]
#![feature(new_uninit)]

use std::{iter, time::Duration};

use criterion::*;
use mem::*;
use rand::Rng;

fn sum(c: &mut Criterion) {
    let mut group = c.benchmark_group("sum");
    let mut rng: rand::rngs::SmallRng = rand::SeedableRng::seed_from_u64(42);
    // let mut group = c.benchmark_group("sum");
    const R: usize = 1024;
    const C: usize = 512;

    let m = Box::<Matrix<R, C>>::new_zeroed();
    let mut m = unsafe { m.assume_init() };
    m.fill_with(|_: usize, _: usize| -> f64 { rng.gen_range(1.0..2.0) });

    group.bench_function("sum row major", |b| b.iter(|| m.sum_all_row_major()));
    group.bench_function("sum col major", |b| b.iter(|| m.sum_all_col_major()));

    group.finish();
}

criterion_group!(benches, sum);
criterion_main!(benches);
