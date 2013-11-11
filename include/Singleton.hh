// $Id: Singleton.hh,v 1.5 2013-11-06 00:23:35 jeremy Exp $

#ifndef SLIC_SINGLETON_HH
#define SLIC_SINGLETON_HH 1

/**
 * @class Singleton<T>
 * @note A templated class for creating Singletons.
 */
template<class T>

class Singleton {

public:

    /**
     * Get the singleton instance.
     */
    static T* instance();

    /**
     * Delete the current singleton instance.
     */
    void release();

    /**
     * Class constructor.
     */
    Singleton();

    /**
     * Class destructor.
     */
    ~Singleton();

private:
    static T* m_singleton;
};

template<class T> T* Singleton<T>::m_singleton = 0;

template<class T>
Singleton<T>::Singleton() {
}

template<class T>
Singleton<T>::~Singleton() {
}

template<class T>
T* Singleton<T>::instance() {
    if (m_singleton == 0) {
        m_singleton = new T;
    }
    return m_singleton;
}

template<class T>
void Singleton<T>::release() {
    if (m_singleton == 0)
        return;
    delete m_singleton;
    m_singleton = 0;
}

#endif

