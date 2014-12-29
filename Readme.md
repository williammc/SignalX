SignalX (a modification of [Nano Signals Slots library](https://github.com/NoAvailableAlias/nano-signal-slot))
================

C++11 Signals and Slots

# How to use
## Declare
```
// Declare sigx::Signals using function signature syntax
sigx::Signal<bool(const char*)> signal1;
sigx::Signal<bool(const char*, std::size_t)> signal2;
```

## Connect
```
// Connect member functions to sigx::signals;
signal1.connect<Sample, &Sample::handler_a>(&sample);
signal2.connect<Sample, &Sample::handler_b>(&sample);

// Connect a static member function
signal1.connect<Sample::handler_c>();

// Connect a free function
signal2.connect<handler_d>();
```

## Emit
```
// Emit Signals
signal1("signal 1 emit");
signal2("signal 2 emit", __LINE__);

std::vector<bool> status;

// Emit Signals and accumulate SRVs (signal return values)
signal1("signal 1 emit again", [&](bool srv)
{
	status.push_back(srv);
});
```

#### Disconnect
```
_Additionally test convenience overloads for references._

// Disconnect member functions from sigx::Signals
signal1.disconnect<Sample, &Sample::handler_a>(sample);
signal2.disconnect<Sample, &Sample::handler_b>(sample);

// Disconnect a static member function
signal1.disconnect<Sample::handler_c>();

// Disconnect a free function
signal2.disconnect<handler_d>();
```

## Connection Management

_To utilize automatic connection management you must inherit from sigx::Observer_

```
struct Sample : public sigx::Observer {
  bool handler_a(const char* e) const { 
    std::cout << e << std::endl;
    return true;
  }

```

## Function Objects

_Because of possible misuse, function objects will not be supported by default._

```
... // add the following to sigx::Function<T_rv(Args...)>

    template <typename L>
    static inline Function bind(L* pointer) {
        return { pointer, [](void *this_ptr, Args... args) {
        return (static_cast<L*>(this_ptr)->operator()(args...)); }};
    }
```
```
... // add the following to sigx::Signal<T_rv(Args...)>

    template <typename L>
    void connect(L* instance) {
        Observer::insert(Function::template bind(instance));
    }
...
    template <typename L>
    void disconnect(L* instance) {
        Observer::remove(Function::template bind(instance));
    }
```