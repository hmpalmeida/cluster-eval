#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

/**********************************************************************
 * TYPEDEFS
 **********************************************************************/

typedef unsigned int uint;

/**Make a N x N matrix.
 *
 * Care must be taken free'ing matrixes create using this function.
 * Correction: just don't delete[] this matrix. If you **REALLLY** want
 * to free this matrix you've got to proceed as follows:
 *
 * 	uint** m = makeSquareMatrix(n); 
 * 	uint* store = &M[0][0]
 * 	uint* m_rows = &M[0]; 
 * 	delete[] store;
 * 	delete[] m_rows;
 *
 * Or just call freeSquareMatrix()
 *
 * @return The square matrix or NULL on non-fatal errors.
 *
 * @warning May throw an exception if allocation fails.
 *
 * @see freeSquareMatrix(), SquareMatrix
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

/**********************************************************************
 * MAIN APP LOGIC
 **********************************************************************/

void myDummyFunction(size_t n)
{
	SquareMatrix<uint> m(1000);
}


int main(int argc, char* argv[])
{
	size_t n = 100;


	myDummyFunction(n);
	myDummyFunction(n);
	myDummyFunction(n);

	uint** matrix = NULL;
	SquareMatrix<uint> m(1000);
	matrix = m;
	if (matrix == NULL) {
		std::cerr << "Error creating square matrix" << std::endl;
		return 1;
	}
}
