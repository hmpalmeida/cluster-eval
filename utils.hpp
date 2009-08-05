/*
 * Utilitary library
 */

#include <string.h>

typedef unsigned int uint;

/*
template <typename T>T** makeSquareMatrix(size_t n);

template <typename T> void freeSquareMatrix(T** m);
*/

template <typename T>
T** makeSquareMatrix(size_t n)
{
	if (n == 0) {
		return NULL;
	}

	T* store = NULL;
	T** m_rows = NULL;
	size_t store_size = n * n * sizeof(T);

	// Allocate matrix
	store = new T[n*n];
	// Clear it...
	memset(store, 0x00, store_size);
	// Setup row pointers
	m_rows = new T*[n];
	for (size_t row = 0; row < n; ++row) {
		m_rows[row] = &store[row * n];
	}

	return m_rows;

}

/**Properly free memory used for a matrix created with makeSquareMatrix.
 *
 * @param m Square matrix to be freed.
 */
template <typename T>
void freeSquareMatrix(T** m)
{
	if (m == NULL) {
		return;
	}

	T* store = &m[0][0];
	T** m_rows = m;

	delete[] store;
	delete[] m_rows;
}

/**RAII object for using matrix created with makeSquareMatrix() easier to use.
 *
 * Creation and de-allocation are handled by this class instances and must not
 * be done by the user itself.
 *
 * This class is non-copyable and non-default-constructible
 */
template <typename T>
class SquareMatrix{
private:
	SquareMatrix();
	SquareMatrix(const SquareMatrix&);
	SquareMatrix& operator=(const SquareMatrix&);

	size_t _n;
	T** _matrix;
public:
	SquareMatrix(size_t n) : _n(n), _matrix(0) {
		_matrix = makeSquareMatrix<T>(_n);
	}

	inline operator T**() { return _matrix; }

	~SquareMatrix() {
		if (_matrix) {
			freeSquareMatrix(_matrix);
			_matrix = NULL;
		}
	}
};


void squareMatrixMultiplication(float** m, float** ms, uint n);

float sumElementsSquareMatrix(float** m, uint n);
