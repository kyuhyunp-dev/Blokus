#ifndef TEST_BASE_HPP
#define TEST_BASE_HPP   

#include <gtest/gtest.h>
#include "shared/PolyominoUtil.hpp"

class PolyominoTestBase : public ::testing::Test
{
protected:
    // GoogleTest calls this once per test suite file execution
    static void SetUpTestSuite()
    {
        if (!sLibraryInitialized)
        {
            sLibrary = sGenerator.getData();
            sLibraryInitialized = true;
        }
    }

    inline static PolyominoDefinition sLibrary;
    inline static PolyominoGenerator sGenerator;
    inline static bool sLibraryInitialized;
};

#endif