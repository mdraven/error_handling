/* тут Val у всех шаблонов последний. Но будет удобнее сделать его первым:
   так легче делать перестановку ошибок.
     Уже не у всех :( у более поздних Val первый.
   Перестановка ошибок, на самом деле, это предикат, который принимает два
   списка, вычитает из второго первый и смотрит остались элементы или нет.
   Вычитание у нас уже используется в if_err, но там вычитается только один
   элемент => его можно использовать для вычитания всех.

   можно использовать boost::mpl::set
*/

/*
  Вообще возможно сделать передачу нескольких ошибок в одном Ret<...>.
  Но идея очень аморфная и в ближайшее время лучше её не трогать, тем
  более тоже самое делается тьюплом(просто пользоваться чуть менее удобно).

  Как это представляю: Ret<Val, ErrA, ErrA> -- два ErrA, каждая проверка один ErrA удаляет.
 */

/* функция разбивания Ret<Val,...> на два Ret<Val,...> с одинаковым типом значения, но
   разными типами ошибок. (а если значением будет ошибка которая есть в одном Ret<...>,
   а в другом Ret<...> такого типа не будет, как делить?)
     Пока не придумал, что делать с проблемой. Но я думают, что добавление возможности
     группировки ошибок для UnOp в if_err покроет все use-case'ы разбивания.
*/

/* можно сделать обёртку ref<Ret<Val,...>, Ref<Val,...>>::type -- тоже самое что и обычная lvalue ссылка, но
   автоматически переставляет типы(иначе ссылка вообще не работает).
           Ненужно, есть decltype */

/* Вспомогательные типы: T(universum), N(nil), V(void) */
struct T {}; // это полное определение T
struct V {}; // это полное определение V

struct N {  // Такой тип нельзя создать (TODO: может просто сделать struct N; без определения?)
    N() = delete;
    N(const N&) = delete;
};

/* V используется, когда нужно сказать, что функция возвращает ничего или ошибку
   (те обычный void): Ret<V, ErrA, ErrB>

   T используется, когда функция возвращает какой-то тип или ошибку:
     Ret<T, ErrA, ErrB>
   причём в отличии от V, который является самым обычным типом с точки зрения семантики,
   к типу T можно скастовать любой тип:
     Ret<T, ErrA, ErrB> func() {
       if(...) return Ret<Int, ErrA>(...);
       else    return Ret<Str, ErrB>(...);
     }
   Если изначальный тип(например Ret<Int, ErrA>) имел значение Int, то Ret<T, ErrA, ErrB>
   получает значение T. Иными словами Int недоступен из Ret<T, ErrA, ErrB>, у нас есть
   просто флаг, что Int был -- этот флаг T.
   T нужен чтобы можно было возвращать информацию об ошибках при разных типах Val.
   Нужно запретить использовать T в качестве типа ошибки: это может вызвать
     конфликт с if_err<T>

   N -- nil. Пустое множество, причём значение N нельзя создать -- это важно.
   Ret<N> создать можно(v -- нет, конструктор Ret() -- пустой).
   Ret<N, ...> можно создать, но у него нет конструктора Ret().
   Он применяется например с if_err<>:
     Ret<Val, ErrA, ErrB> v = ...;
     Ret<Val> ret = if_err<T>(std::move(v), [](Ret<N, ErrA, ErrB>& err) {});
   мы уже знаем что в правой части if_err нет Val(там только ошибки), поэтому
   там стоит тип N -- нет значения.
   N -- самый частный случай, его можно присвоить Val если есть ошибки: Ret<Val, ErrA>() = Ret<N, ErrA>();
     Если ошибок нет, то нельзя: Ret<Val>() = Ret<N>() <- error у N нет значения.
     Такой косяк связан с тем, что с одной стороны Ret<N> должен существовать(чтобы поглотить значение),
     с другой стороны N ещё меньшее значение чем Void.
   Но N нельзя присвоить ничего кроме N.
   У N особая форма Ret<N> -- в ней нет способа извлечь N.
   Кроме того возможна оптимизация в случае if_err<Err>(Ret<N, Err>(), ...)
     так как заведомо известно, что там Err(N содержаться не может).

*/

/* польза от N:
     Ret<N, ErrA, ErrB, ErrC> old_err_codes_to_new(int err_code) {
       if(err_code == 1)
         return ErrA;
       if(err_code == 2)
         return ErrB;
       if(err_code == 3)
         return ErrC;
     }
   Если код ошибки уже получен, то мы можем вызвать эту функцию и переконвертировать
   из старого формата в новый. Мы вызываем эту функцию(и другие)
   из функции Ret<Val, ErrA, ..., ErrD, ErrE> func(); -- как видно ошибки и значения
   сложились(N можно присвоить Val, Ret тоже присваивается).   
*/

struct A; /* Класс/структура ошибки. Наследовать ни от чего не обязательно, обычный класс.
             'A' должен обязательно быть перемещаемым.
           */

class Val; /* Если можно копировать, то Ret<Val> тоже можно копировать.
              Ret<...,Val> можно только перемещать.
              Val должен обязательно быть перемещаемым.
            */

template <class Val>
class Ret final { // используется когда все ошибки удалены
    Val v;
public:
    Ret(); // создаёт Val по-умолчанию
    Ret(const Val& v); /* убрал explicit, потому что писать return Ret<Val>(v); неудобно.
                          Но может есть причина, чтобы вернуть.
                          Если Val нельзя копировать, то этого конструктора нет. */
    Ret(Val&& v) noexcept; /* сравнить с конструктором копирования для Val.
                            Надо написать noexcept(noexcept(Val(Val()))) или что-то такое,
                            но пока что это будет захламлять описание.*/

    Ret(const Ret<Val>& v); /* кажется тут не нужен explicit */
    Ret(Ret<Val>&& v) noexcept; /* сравнить с конструктором копирования Ret<Val> */

    Ret<Val>& operator=(const Val& v);
    Ret<Val>& operator=(Val&& v) noexcept;

    Ret<Val>& operator=(const Ret<Val>& v); /* можно присваивать только Ret<Val>.
                                               Присваивание Ret<...,Val> невозможно, так
                                               как Ret<Val> самое строгое значение */
    Ret<Val>& operator=(Ret<Val>&& v) noexcept; /* сверить с оператором копирования */

    explicit operator(Val&)() noexcept; /* 1)над explicit надо подумать, может лучше убрать.
                                  2)стоит ли возвращать ссылку? продлит ли жизнь выражение вида:
                                      funct(Ret<Val>()); // void funct(Ret<Val>& x);
                                    ?
                                         ссылку возвращать стоит так как в таких функциях
                                         prvalue всё равно будет через ссылку.
                                  3)noexcept так как возвращаем ссылку. Что может случиться?
                                  4)оператор конвертирования нужен, чтобы работал трейт is_convertible,
                                    но я не проверил в самом деле будет true или нет :(
                                */
    Val& data();              /* возвращают ссылку на Val. Они нужны так как кастовать неудобно,
                                 а делать это нужно часто. */
    const Val& data(); const;

    /* тут набор операторов сравнения, если они есть у Val.
       Все сравнения только с Val и Ret<Val>, никаких Ret<...,Val> */

    ~Ret() = default; // ничего необычного
}

// Ret для пустого типа N
template <>
class Ret<N> final {
public:
    Ret() noexcept = default;

    Ret(const Ret<N>&) noexcept = default;
    Ret(Ret<N>&&) noexcept = default;

    Ret<N>& operator=(const Ret<N>&) noexcept = default;
    Ret<N>& operator=(Ret<N>&&) noexcept = default;

    /* операторов сравнения у N -- нет */

    /* извлечь N нельзя */

    ~Ret() noexcept = default;
};

// Ret для универсума T
template <>
class Ret<T> final {
public:
    Ret() noexcept = default;

    // (*) Любой Val создаёт T; сам Val не участвует, поэтому noexcept безусловный

    template <class Val> // (*)
    Ret(const Val& v) noexcept;

    template <class Val> // (*)
    Ret(Val&& v) noexcept;

    template <class Val> // (*)
    Ret(const Ret<Val>& v) noexcept = default;

    template <class Val> // (*)
    Ret(Ret<Val>&& v) noexcept = default;

    template <class Val> // (*)
    Ret<T>& operator=(const Val& v) noexcept;

    template <class Val> // (*)
    Ret<T>& operator=(Val&& v) noexcept;

    template <class Val> // (*)
    Ret<T>& operator=(const Ret<Val>& v) noexcept = default;

    template <class Val> // (*)
    Ret<T>& operator=(Ret<Val>&& v) noexcept = default;

    /* У T нет операторов сравнения, он вездесущь */

    ~Ret() = default; // ничего не делает
};


template <class... Args> /* В Args вначале идут ошибки Err0, Err1, ..., потом Val.
                            Далее Err0, ... буду обозначать ErrN.
                         */
class Ret {
    boost::any v; // надо будет взять что-то побыстрее, но пока сойдёт и это
public:
    Ret(); /* создаёт Val по-умолчанию.(Почему Val, а не ErrN?
              Может стоит убрать этот конструктор?)
           */
    Ret(const Val& v); /* убрал explicit, потому что писать return Ret<Val>(v); неудобно.
                          Но может есть причина, чтобы вернуть */
    Ret(Val&& v) noexcept; /* сравнить с конструктором копирования для Val.
                              Надо написать noexcept(noexcept(Val(Val()))) или что-то такое,
                              но пока что это будет захламлять описание.*/
    Ret(const ErrN& v); /* сравнить с конструктором копирования для Val.
                           Должен быть для всех ошибок. */
    Ret(ErrN&& v) noexcept; /* сравнить с конструктором копирования для Ret.
                               Должен быть для всех ошибок. */

    Ret(const Ret<Args...>& v) = delete; /* кажется тут не нужен explicit.
                                            Этот тип можно только перемещать: деструктор этого класса
                                            "убивает" программу, и избежать этого можно только "очистив"
                                            Ret<Args...> до Ret<Val> */
    Ret(Ret<...,Val>&& v) noexcept; /* сравнить с конструктором копирования Ret<Args...>.
                                       незнаю как выразить параметры у Ret<...,Val>, но там должны
                                       быть все возможные комбинации типов ошибок(в том числе с
                                       некоторыми отсутствующими). Разумеется у перестановок в Ret<...,Val>
                                       Val всегда в конце.
                                       Из-за того что тип где присутствуют те же ошибки в том же порядке
                                       (такой Ret<...,Val> один) смешан с другими -- мы не можем сделать
                                       этот метод "= default" а значит не попадаем в какую-то(я не помню
                                       какую) категорию в c++. Это плохо. Надо вынести этот тип в отдельный
                                       метод.
                                       Кстати, необязательно чтобы Val у обоих совпадал: если можно неявно
                                       привести тип, то OK.
                                    */

    Ret<Args...>& operator=(const Val& v); /* Присваивать Val можно, так как параметр Val у Ret есть всегда.
                                              Т.е. Val нельзя убрать проверками и поэтому можно присвоить */
    Ret<Args...>& operator=(Val&& v) noexcept; /* сравнить с оператором присваивания Val
                                                */
    Ret<Args...>& operator=(const ErrN& v); /* Можно присваивать ErrN, если он есть в Args,
                                              так как его ещё не проверили: иначе его бы в Args не было */
    Ret<Args...>& operator=(ErrN&& v) noexcept; /* сравнить с оператором присваивания ErrN
                                                */

    Ret<Args...>& operator=(const Ret<Args...>& v) = delete; /* Присваивать нельзя, можно только перемещать:
                                                                деструктор класса "убивает" программу, если у него
                                                                есть v */
    Ret<Args...>& operator=(Ret<...,Val>&& v) noexcept; /* сверить с оператором копирования.
                                                           Ret<...,Val> такое же как у конструктора перемещения
                                                           Ret<...,Val>: все возможные комбинации типов ошибок из
                                                           Args(в том числе без некоторых). Иными словами Ret<...,Val>
                                                           более строгий чем Ret<Args...> */

    /* операторов приведения типа(например к Val или ErrN) -- нет: если тип в v не совпал, то
       мы можем только бросить исключение, но эта библиотека не кидает >своих< исключений(возможно только в
       деструкторе этого класса) */

    /* набор операторов сравнения отсутствует, так как всё что они могут при
       v != Val -- кинуть исключение, а кидать исключения нельзя(в этом суть идеи) */

    ~Ret(); /* если в v что-то есть, то завершает программу с текстом ошибки(может лучше кидать исключение?)
                  (лучше не завершать программу, и не кидать исключение в деструкторе, так как это не позволит
                   кидать исключение в других частях программы(вызовется этот "плохой" деструктор).
                   Надо сделать макрос, который принимает тип Ret<...> и пользователь может его переопределить
                   как хочет. По-умолчанию там будет printf(или cout))
               иначе ничего особенного не делает */
};

// небезопасная функция для доступа к кишкам Ret<...>
// возвращаемый тип -- неопределён, а не boost::any. Так что нормально её могу использовать
// только я ^_^
template <class Val, class... Errors>
boost::any& unsafe_access_to_internal_data(Ret<Val, Errors...>& v) {
	return v.v;
}

// unsafe для Ret<Val>(он возвращает не any)
template <class Val>
Val& unsafe_access_to_internal_data(Ret<Val>& v) {
	return v.v;
}

// у boost::any нет незащищённого варианта, но у меня есть такие места
// где защита не нужна. Буду использовать эту заглушку.
template <class Val>
Val unsafe_any_cast(Any& v) {
	return boost::any_cast<Val>(v);
}

// Помошник для if_err. Например нам надо присвоить Ret<Val> то что лежит в Ret<Val, Err>:
// мы не сможем это сделать так как в Ret<Val, Err> лежит Any, а в Ret<Val> сам Val.
//
// Этот помошник берёт на себя работу по диспатчу присваиваний(легкое:
// Ret<Val, ErrA> -> Ret<Val, ErrB>, и более сложное: Ret<Val, ErrA> -> Ret<Val>).
//
// Этот хелпер очень тонкий, он не знает особенностей Ret<...>, не нужно делать
// тут проверки на возможность присвоить. Максимум что он делает -- это конвертирует
// и решает: перемещать или копировать, больше ничего он не должен делать!
//
// Это очень опасный класс, он нарушает правила Ret<...>, поэтому он должен
// быть другом ограниченного набора классов(идельно: только if_err). Если
// сделать его публичным, то можно будет "не светить" unsafe_any_cast, а
// использовать его.
class AssignHelperForIfErr {
	template <class Err, class UnOp, class Val, class... Errors>
	friend
	typename RetTypeFor_IfErrValErrors<Err, Val, Errors...>::type
	if_err(Ret<Val, Errors...>&& v, UnOp op);

	template <class Val, class OVal, class... OErrors>
	static void assign(Ret<Val>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(unsafe_any_cast<Val>(unsafe_access_to_internal_data(ov)));
	}

	template <class Val, class Err, class... Errors, class OVal, class... OErrors>
	static void assign(Ret<Val, Err, Errors...>& v, Ret<OVal, OErrors...>&& ov) {
		unsafe_access_to_internal_data(v) = std::move(unsafe_access_to_internal_data(ov));
	}
};

// Пока что if_err надо сделать другом Ret<Args...> (но не Ret<Val>!)
//      Сейчас можно не делать другом, есть unsafe_access_to_internal_data.
template <class Err, class UnOp, class... Args>
auto if_err(Ret<Args...>&&/* тут не универсальная ссылка! */
            err,
            UnOp  /* унарный оператор.
                     тут подразумевается, что он принимает Err&&(не универсальную ссылку!)
                     возможно стоит сделать явную проверку, что он принимает Err&&
                  */
            func) -> Ret<MAGIC_FILT<Err, Args...>> { /* MAGIC_FILT убирает из Args тип Err */
    if(err.v.type == typeid(Err)) {
        // если func возвращает Ret<...>, то: return func(std::move(err.v));
        // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
        /* если func возвращает void: func(std::move(err.v)); return Ret<Val>();
           (такое поведения для типов Val != Void нужно потому, что из лямбды могут кидать
           исключение или делать exit(). И если разрешить не писать типы только для Val == Void,
           то тип придётся писать и в случаях когда мы не выходим из лямбды нормально, а это неудобно) */
    }
    return std::move(err.v);
}

// форма if_err<T> -- передаёт в func значение типа Ret<N, Errors...>
// возвращает Ret<Val>
template <class UnOp, class Val, class... Errors>
auto if_err<T, UnOp, Errors...>(Ret<Val, Errors...>&& err,
                                UnOp func) -> Ret<Val> {
    if(err.v.type == typeid(Val))
        return std::move(err.v);
    // если func возвращает Ret<...>, то: return func(std::move(err.v));
    // итд как в обобщённом if_err
}

/* форма if_err в которую передают Ret<Val>. Отбрасывает параметр func и возвращает
   то, что передали.
   Нужна для облегчения написания обобщенного кода.
      Учитывая, что буду делать с переменным числом UnOp, то
      эта функция не должна принимать UnOp вообще.
 */
template <class Val, class UnOp>
auto if_err(Ret<Val>&& val, UnOp func) -> Ret<Val> {
    return std::move(err.v);
}

/* форма if_err для N и одной ошибки
   она для оптимизации: мы уже знаем, что там Err, а не N */
template <class Err, class UnOp>
auto if_err(Ret<N, Err>&& val, UnOp func) -> Ret<N> {
    // если func возвращает Ret<N>, то: return func(std::move(err.v));
    // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
    // если func возвращает void: func(std::move(err.v)); return Ret<N>();
}


/* форма if_err которая принимает несколько ошибок: if_err<ErrA, ErrB>
   спорная: внутри придётся делать if(err.v.type == typeid(ErrA) || err.v.type == typeid(ErrB) ...)
   это не очень эффективно с точки зрения скорости(наверное).
   В любом случае такую форму можно сделать когда угодно, а убрать не просто(хотя она и удобная).
   Эта форма передаёт в func Ret<N, перечисленные ErrN> а возвращает Ret<Val, всё что осталось>
      Эта форма может быть быстрее из-за if-else. И если её реализовать, то стоит сделать
      возможными группировки(пока синтаксис хорошо не продумал и пока вижу так:
        ... = if_err<Ret<N, ErrA, ErrB>, Ret<N, ErrC, ErrD>, ErrE>(std::move(xxx),
                [](Ret<N, ErrA, ErrB>&&) {},
                [](Ret<N, ErrC, ErrD>&&) {},
                [](ErrE&&) {});
      )
*/

/* вообще-то if_err не обязательно принимать список ошибок: он уже есть в Ret<Val, Errors...>.
   Поэтому можно просто перебирать операторы и подставлять типы ошибок в них пока они не подойдут.
      С этим много гемороя и лучше оставить на потом. */

/* очень важная мысль: if_err позволяет в возвращаемом типе менять ошибки, а repack тип возвращаемого
   значения, и никак не иначе. Если if_err поменяет тип возвр-го значения, то что делать,
   если ошибки не было? Что положить в новый тип, если в старом лежит значение, а не ошибка?
   Те же вопросы, но касательно ошибок для repack */

/* если сделать чтобы if_err принимал несколько UnOp, то можно выбирать их подставляя в них код
   ошибки поочереди. Тогда можно будет использовать такие функторы:
     struct Op {
        Ret<...> operator(ErrA&&)();
        Ret<...> operator(ErrB&&)();
        Ret<...> operator(ErrC&&)();
        Ret<...> operator(Ret<N, ErrA>&&)();
     };
   главное, после нахождения подходящего типа продолжить перебор типов: если есть ещё подходящие, то
   UnOp не удаляется, а передаётся на следующую итерацию */



/* функция для перепаковки. Вынимает Val и передаёт в UnOp.
   После этого может вернуть новый тип OVal, и новые ошибки OErrors, но(!)
   новый тип может быть абсолютно любым(Val мы уже обработали), а вот OErrors должны
   включать все ошибки из Errors(так как их мы не обрабатывали).
   OVal может быть: T -- неважен тип, главное обобщённость; N -- в Ret<...> обязательно
   ошибка; V -- просто хочу просигнализировать, что необязательно ошибка.
*/
template <class OVal, class... OErrors, class UnOp, class Val, class... Errors>
auto repack(Ret<Val, Errors...>&& val, UnOp func) -> Ret<OVal, OErrors...> {
    if(val.v.type == typeid(Val)) {
        // если func возвращает Ret<...>, то: return func(std::move(val.v));
        // если func возвращает что-то другое, то ошибка компиляции(зарезервирую bool для себя ^_^)
        // если func возвращает void: func(std::move(val.v)); return Ret<OVal>();
    }

    return std::move(val.v);
}

/* операторы для обобщённого программирования */

template <class... Errors> // операции над кодами ошибок; реализация не нужна
struct ErOp;

template <class Val> // операции над возвращаемым типом Val; реализация не нужна
struct ValOp;

// Добавить типов ошибок. Если такие ошибки уже есть, то добавляются те которых нет.
// Реализация не нужна так как используется только в decltype
// Коммутативность не нужна, будет проще читать если Ret всегда слева, а ErOp всегда справа.
template <class Val, class... Errors, class... OErrors>
auto operator+(Ret<Val, Errors...>, ErOp<OErrors...>) -> Ret<Val, UNION(Errors..., OErrors...)>;

// Убрать некоторые типы ошибок. Реализация не нужна. Ошибок времени компиляции -- нет.
template <class Val, class... Args, class... Errors>
auto operator-(Ret<Val, Args...>, ErOp<Errors...>) -> Ret<Val, DIFFERENCE(Args..., Errors...)>;

// Меняет тип возвращаемого значения. Проверок никаких не нужно, это
// всего лишь тип, а проверки будут когда кто-то захочет создать/присвоить значение.
// Реализация не нужна, используется в decltype.
template <class Val, class... Args, class NewVal>
auto operator=(Ret<Val, Args...>, ValOp<NewVal>) -> Ret<NewVal, Args...>;

// Оператор для проверки наличия типа ошибки. Возвращает истину
// если все Errors... есть в Args..., иначе ложь.
// Символ '<=' потому что слева более слабое или равное условие чем справа
// (это проверка ошибок, поэтому про Val == N можно не думать).
template <class Val, class... Args, class... Errors>
auto operator<=(Ret<Val, Args...>, ErOp<Errors...>) -> IN(Args..., Errors...);

// Оператор для проверки можно ли привести OVal к Val.
// Иными словами это проверка is_convertible
template <class Val, class... Errors, class OVal>
auto operator<=(Ret<Val, Errors...>, ValOp<OVal>) -> bool;

/* Если у нас есть много ошибок(например из boost::asio), то
   размер типа Ret<...> будет огромным и нам придётся кругом писать
   usage XXX = Ret<...>, что не всегда наглядно.
   С помощью операторов для обобщённого программирования можно решить
   эту проблему: мы делаем usage AsioErrors = ErOp<тут ошибки>;,
   а потом прибавляем их к Ret: decltype(Ret<Val, другие ошибки> + AsioErrors);
*/

// ------------------------- ПРИМЕР -------------------

// Val -- хорошее значение, оно всегда последнее(TODO: может лучше сделать первым?)
// A, B, C -- типы ошибок; порядок не имеет значения
Ret<A, B, C, Val> func() {
    return A();
    // return B("yay!");
    // return Val();
}

Ret<B, C, Val> checkA(Ret<A, B, C, Val>&& v) {
    Ret<B, C, Val> ret = if_err<A>(std::move(v), [](A&& a) {/* обработка */});

#if 0
    // Ret<...,Val> в деструкторе пишет, что не всё проверено и валит программу.
    // Ret<Val> отрабатывает нормально. => нужно всё перемещать

    Ret<DivOnZero, Int> v = Int(10) / Int(0);
    Ret<Int> ret = if_err<DivOnZero>(std::move(v), [](DivOnZero&&) -> Ret<Int> { return Int(Inf); });

    Ret<DivOnZero, Int> v = Int(10) / Int(0);
    Ret<Int> ret = if_err<DivOnZero>(std::move(v), [](DivOnZero&&) -> void { exit(1); });

    Ret<A, B, C, Val> v = func();
    Ret<B, C, Val> ret = if_err<A>(std::move(v), [](A&&) -> Ret<C, Val> { return bla-bla; }); /* Можем вернуть меньше ошибок,
                                                                                                 но не можем больше */

    Ret<A, B, C, D, E, Val> v = func(); // присваивание менее строгому
    //! Ret<A, B, Val> v = func(); // присваивание более строгому -- ошибка

    Ret<A, B, Val> v = Val(); // OK

    Ret<A, Void> v = ret_void(); // функция возвращает только ошибки; значение void

    Ret<A, B, C, Val> v1 = func();
    Ret<D, Void> v2 = ret_void();
    return std::make_tuple(std::move(v1), std::move(v2)); // объединяем разные ошибки

    Ret<Val> v = Val();
    auto ret = if_err<Val>(std::move(v), [](Val&) {}); // ошибка, нельзя if_err<Val>

    //! Ret<Val> v = Ret<N>(); // ошибка: в N нет значения
    //! Ret<Val, ErrA> v = Ret<N>(); // ошибка: в N нет значения
    //! Ret<N> v = Ret<Val>(); // ошибка: N можно присвоить только N
    //! Ret<N, ErrA> v = Ret<Val>(); // ошибка: N можно присвоить только N
    //! Ret<N, A> v = Ret<N>(); // ошибка: в N нет значения; в v должен быть A
    Ret<Val, A> v = Ret<N, A>(); // OK: есть ещё A
    Ret<N, A, B> v = Ret<N, A>(); // OK: есть ещё A

    if_err<ErrB>(std::move(xxx), [](ErrB&&) -> Ret<std::string, ErrA> {}); // должен падать из-за не поглащонной ошибки
    if_err<ErrB>(std::move(xxx), [](ErrB&&) {}); // скорее всего не падает, так как возвращает Ret<Val>
#endif

    return ret;
}

Ret<Val> checkBC(Ret<B, C, Val>&& v) {
    Ret<C, Val> ret1 = if_err<B>(std::move(v), [](B&& b) {/* обработка */});
    Ret<Val> ret2 = if_err<C>(std::move(ret1), [](C&& c) {/* обработка */});

    return ret2;
}


int main(int argc, char *argv[]) {
    Ret<A, B, C, Val> v1 = func();
    Ret<C, B, Val> v2 = checkA(std::move(v1)); // Переместили коды ошибок
    Ret<Val> v3 = checkBC(std::move(v2));

    std::cout << static_cast<Val>(v3) << std::endl;

    return 0;
}
