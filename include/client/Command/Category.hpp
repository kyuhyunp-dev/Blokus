#ifndef CATEGORY_HPP
#define CATEGORY_HPP

namespace Category
{
    enum Type
    {
        None = 0,
        Scene = 1 << 0,
        ActivePiece = 1 << 1,
        Arena = 1 << 2,
        Board = 1 << 3,
    };
};

#endif