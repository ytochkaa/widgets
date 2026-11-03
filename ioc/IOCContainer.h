#ifndef IOCCONTAINER_H
#define IOCCONTAINER_H

#include <functional>
#include <map>
#include <memory>

/**
 * @brief IoC-контейнер для внедрения зависимостей.
 *
 * Хранит для каждого интерфейса фабрику, которая создаёт его реализацию.
 * Классы получают зависимости через контейнер и не знают о конкретных реализациях.
 */
class IOCContainer
{
    /// Счётчик для выдачи идентификаторов типам.
    inline static int nextTypeId = 115094801;

    /**
     * @brief Возвращает уникальный идентификатор типа T.
     * @tparam T тип, для которого нужен идентификатор.
     * @return идентификатор типа.
     */
    template <typename T>
    static int GetTypeID()
    {
        static int typeId = nextTypeId++;
        return typeId;
    }

public:
    /**
     * @brief Базовый класс для всех фабрик.
     */
    class FactoryBase
    {
    public:
        virtual ~FactoryBase() = default;
    };

    /// Зарегистрированные фабрики по идентификатору типа.
    std::map<int, std::shared_ptr<FactoryBase>> factories;

    /**
     * @brief Фабрика, создающая объект типа T.
     * @tparam T тип создаваемого объекта.
     */
    template <typename T>
    class CFactory : public FactoryBase
    {
        std::function<std::shared_ptr<T>()> functor;

    public:
        explicit CFactory(std::function<std::shared_ptr<T>()> functor) : functor(functor) {}

        /**
         * @brief Создаёт объект.
         * @return созданный объект.
         */
        std::shared_ptr<T> GetObject() { return functor(); }
    };

    /**
     * @brief Возвращает объект, зарегистрированный для типа T.
     * @tparam T тип (интерфейс) объекта.
     * @return объект или nullptr, если тип не зарегистрирован.
     */
    template <typename T>
    std::shared_ptr<T> GetObject()
    {
        auto it = factories.find(GetTypeID<T>());
        if (it == factories.end()) {
            return nullptr;
        }
        auto factory = std::static_pointer_cast<CFactory<T>>(it->second);
        return factory->GetObject();
    }

    /**
     * @brief Регистрирует функтор, создающий объект из его зависимостей.
     * @tparam TInterface тип интерфейса.
     * @tparam TS типы зависимостей.
     * @param functor функтор создания объекта.
     */
    template <typename TInterface, typename... TS>
    void RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TS>... ts)> functor)
    {
        factories[GetTypeID<TInterface>()] =
            std::make_shared<CFactory<TInterface>>([=] { return functor(GetObject<TS>()...); });
    }

    /**
     * @brief Регистрирует уже созданный объект (один на всё приложение).
     * @tparam TInterface тип интерфейса.
     * @param t объект.
     */
    template <typename TInterface>
    void RegisterInstance(std::shared_ptr<TInterface> t)
    {
        factories[GetTypeID<TInterface>()] = std::make_shared<CFactory<TInterface>>([=] { return t; });
    }

    /**
     * @brief Регистрирует фабрику: при каждом запросе создаётся новый объект TConcrete.
     * @tparam TInterface тип интерфейса.
     * @tparam TConcrete конкретная реализация.
     * @tparam TArguments типы зависимостей конструктора.
     */
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterFactory()
    {
        RegisterFunctor(std::function<std::shared_ptr<TInterface>(std::shared_ptr<TArguments>... ts)>(
            [](std::shared_ptr<TArguments>... arguments) -> std::shared_ptr<TInterface> {
                return std::make_shared<TConcrete>(arguments...);
            }));
    }

    /**
     * @brief Создаёт один объект TConcrete и регистрирует его.
     * @tparam TInterface тип интерфейса.
     * @tparam TConcrete конкретная реализация.
     * @tparam TArguments типы зависимостей конструктора.
     */
    template <typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterInstance()
    {
        RegisterInstance<TInterface>(std::make_shared<TConcrete>(GetObject<TArguments>()...));
    }
};

#endif
