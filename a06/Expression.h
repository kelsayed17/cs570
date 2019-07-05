//
// Created by Khaled Elsayed on 2019-05-06.
//

#ifndef DISKSIM_EXPRESSION_H
#define DISKSIM_EXPRESSION_H

#endif //DISKSIM_EXPRESSION_H

template<class T>
class Expression {
public:
    virtual bool interpret(T *context) = 0;

    virtual ~Expression() = default;
};