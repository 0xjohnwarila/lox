package com.craftinginterpreters.lox;

import java.util.List;

abstract class Expr {
  static class Binary extends Expr {
    Binary(Expr lef, Token operator, Expr right) {
      this.lef = lef;
      this.operator = operator;
      this.right = right;
    }

    final Expr lef;
    final Token operator;
    final Expr right;
  }
  static class Grouping extends Expr {
    Grouping(Expr expression) {
      this.expression = expression;
    }

    final Expr expression;
  }
  static class Literal extends Expr {
    Literal(Object value) {
      this.value = value;
    }

    final Object value;
  }
  static class Unary extends Expr {
    Unary(Token operator, Expr right) {
      this.operator = operator;
      this.right = right;
    }

    final Token operator;
    final Expr right;
  }
}