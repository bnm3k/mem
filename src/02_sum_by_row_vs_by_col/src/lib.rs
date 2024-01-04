use std::fmt::Debug;

pub struct Matrix<const R: usize, const C: usize> {
    elems: [[f64; C]; R],
}

impl<const R: usize, const C: usize> std::ops::Index<usize> for Matrix<R, C> {
    type Output = [f64; C];
    fn index(&self, index: usize) -> &Self::Output {
        &self.elems[index]
    }
}
impl<const R: usize, const C: usize> std::ops::IndexMut<usize>
    for Matrix<R, C>
{
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.elems[index]
    }
}

impl<const R: usize, const C: usize> Debug for Matrix<R, C> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if R == 0 {
            return write!(f, "[]");
        }
        write!(f, "[ {:?}", self[0])?;
        for r in 1..R {
            write!(f, "\n  {:?}", self[r])?;
        }
        write!(f, " ]")?;
        Ok(())
    }
}

impl<const R: usize, const C: usize> Matrix<R, C> {
    pub fn fill_with(&mut self, mut f: impl FnMut(usize, usize) -> f64) {
        for r in 0..R {
            for c in 0..C {
                self.elems[r][c] = f(r, c);
            }
        }
    }
    pub fn sum_all(&self) -> f64 {
        self.sum_all_row_major()
    }

    pub fn sum_all_row_major(&self) -> f64 {
        let mut sum = 0.0;
        for r in 0..R {
            for c in 0..C {
                sum += self[r][c];
            }
        }
        return sum;
    }

    pub fn sum_all_col_major(&self) -> f64 {
        let mut sum = 0.0;
        for c in 0..C {
            for r in 0..R {
                sum += self[r][c];
            }
        }
        return sum;
    }
}

impl<const N: usize> Matrix<N, N> {
    pub fn multiply(a: &Matrix<N, N>, b: &Matrix<N, N>, c: &mut Matrix<N, N>) {
        // AC: row-wise B and row-wise C
        for i in 0..N {
            for k in 0..N {
                let r = a[i][k];
                for j in 0..N {
                    c[i][j] += r * b[k][j];
                }
            }
        }
    }
}
