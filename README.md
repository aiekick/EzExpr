# EzExpr

EzExpr is a easy to use and flexible math expression parser / evaluator 
in a one file header only style for cpp11 at least

# Quick Start

```cpp
ez::Expr ev;
auto result0 = ev.parse("x + 2.5 * d").set("x", 0.5).set("d", 1.5).eval().getResult();
auto result1 = ev.set("x", 10.5).eval().getResult();
auto result2 = ev.set("d", 2.5).eval().getResult();
```

# Features

