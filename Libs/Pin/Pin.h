
// Interface
class Pin {
public:
    enum Direction {
        D_INPUT = 0,
        D_OUTPUT = 1,
        D_PULL_UP = 1 << 1,
        D_PULL_DOWN = 1 << 2,
        D_PULL = D_PULL_UP | D_PULL_DOWN
    };

    virtual bool value() const = 0;
    virtual Direction direction() const = 0;
    virtual void setDirection(Direction) = 0;
    virtual void setValue(bool) = 0;
};

// returns 0, set - nothing
class DummyPin : public Pin {
public:
    DummyPin() : Pin() {}
    bool value() const;
    Direction direction() const;
    void setDirection(Direction);
    void setValue(bool);
};
