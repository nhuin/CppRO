#ifndef FIXEDSIZEALLOCATOR_HPP
#define FIXEDSIZEALLOCATOR_HPP

#include <cassert>

template<typename T>
class FixedSizeAllocator {
	private:

	int m_size;
	T* m_objects;

	int m_ptrAlloc;
	T** m_allocation;

	public:

	FixedSizeAllocator(int _size) :
		m_size(_size),
		m_objects((T*)malloc(m_size * sizeof(T))),
		m_ptrAlloc(0),
		m_allocation((T**)malloc(m_size * sizeof(T*)))
	{
		for(int i = 0; i < m_size; ++i) {
			m_allocation[i] = m_objects+i;
		}
		m_ptrAlloc = m_size - 1;
	}

	~FixedSizeAllocator() {
		free(m_objects);
		free(m_allocation);
	}

	FixedSizeAllocator(const FixedSizeAllocator& _other) = delete;

	FixedSizeAllocator& operator=(const FixedSizeAllocator&) = delete;

	FixedSizeAllocator(FixedSizeAllocator&&) = delete;
	FixedSizeAllocator& operator=(FixedSizeAllocator&&) = delete;

	bool canAllocate() const {
		return m_ptrAlloc >= 0;
	}

	T* allocate() {
		assert(m_ptrAlloc > 0);
		return m_allocation[m_ptrAlloc--];
	}

	void deallocate(T* _obj) {
		m_allocation[++m_ptrAlloc] = _obj;
	}

	void deallocateAll() {
		for(int i = 0; i < m_size; ++i) {
			m_allocation[i] = m_objects+i;
		}
		m_ptrAlloc = m_size - 1;
	}
};

#endif