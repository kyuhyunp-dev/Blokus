#include "Command/CommandQueue.hpp"

#include <gtest/gtest.h>


TEST(CommandQueueTest, PushAndPopTest) 
{
    CommandQueue queue;

    // 1. Initial state
    EXPECT_TRUE(queue.isEmpty());

    // 2. Single Push/Pop cycle
    Command cmd1;
    cmd1.category = Category::Arena;
    
    queue.push(cmd1);
    EXPECT_FALSE(queue.isEmpty());

    Command popped1 = queue.pop();
    EXPECT_EQ(popped1.category, Category::Arena);
    EXPECT_TRUE(queue.isEmpty());

    // 3. Double Push/Pop cycle (Verifying FIFO)
    Command cmd2;
    cmd2.category = Category::ActivePiece;
  
    queue.push(cmd2);

    Command cmd3;
    cmd3.category = Category::Board;

    queue.push(cmd3);
    EXPECT_FALSE(queue.isEmpty());

    // First out should be cmd2
    Command popped2 = queue.pop();
    EXPECT_EQ(popped2.category, Category::ActivePiece);
    EXPECT_FALSE(queue.isEmpty());

    // Second out should be cmd3
    Command popped3 = queue.pop();
    EXPECT_EQ(popped3.category, Category::Board);
    EXPECT_TRUE(queue.isEmpty());
}