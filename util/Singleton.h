#pragma once
template <typename T>
class Singleton
{
public:
    template<typename... Args>
    static T* instance(Args&&... args){
        if(m_pInstance==nullptr)
            m_pInstance = new T(std::forward<Args>(args)...);
        return m_pInstance;
    }

    static T* getInstance(){
        return m_pInstance;
    }

    static void destroyInstance()
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }

private:
    Singleton(void);
    virtual ~Singleton(void);
    Singleton(const Singleton&);
    Singleton& operator = (const Singleton&);
private:
    static T* m_pInstance;
};

template <class T> T*  Singleton<T>::m_pInstance = nullptr;

