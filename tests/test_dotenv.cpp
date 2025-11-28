/**
 * @file test_dotenv.cpp
 * @brief Unit tests for Dotenv C++ Library
 */

#include "dotenv.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>

// Simple test framework
#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " << #name << "... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl; \
    passed++; \
} while(0)

#define ASSERT_EQ(expected, actual) do { \
    if ((expected) != (actual)) { \
        std::cerr << "\nAssertion failed: " << #expected << " != " << #actual << std::endl; \
        failed++; \
        return; \
    } \
} while(0)

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        std::cerr << "\nAssertion failed: " << #condition << " is not true" << std::endl; \
        failed++; \
        return; \
    } \
} while(0)

#define ASSERT_FALSE(condition) do { \
    if ((condition)) { \
        std::cerr << "\nAssertion failed: " << #condition << " is not false" << std::endl; \
        failed++; \
        return; \
    } \
} while(0)

static int passed = 0;
static int failed = 0;

// Helper function to create a test .env file
void createTestEnvFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

// Helper function to remove test file
void removeTestFile(const std::string& filename) {
    std::remove(filename.c_str());
}

TEST(basic_load) {
    createTestEnvFile("test_basic.env", 
        "DATABASE_URL=postgresql://localhost/testdb\n"
        "API_KEY=test123\n"
        "PORT=3000\n"
    );
    
    DotenvError result = Dotenv::load("test_basic.env");
    ASSERT_EQ(DotenvError::Success, result);
    ASSERT_EQ(std::string("postgresql://localhost/testdb"), Dotenv::get("DATABASE_URL"));
    ASSERT_EQ(std::string("test123"), Dotenv::get("API_KEY"));
    ASSERT_EQ(std::string("3000"), Dotenv::get("PORT"));
    
    Dotenv::clear();
    removeTestFile("test_basic.env");
}

TEST(quoted_values) {
    createTestEnvFile("test_quoted.env",
        "DOUBLE_QUOTED=\"value with spaces\"\n"
        "SINGLE_QUOTED='another value'\n"
        "NO_QUOTES=simple_value\n"
    );
    
    Dotenv::load("test_quoted.env");
    ASSERT_EQ(std::string("value with spaces"), Dotenv::get("DOUBLE_QUOTED"));
    ASSERT_EQ(std::string("another value"), Dotenv::get("SINGLE_QUOTED"));
    ASSERT_EQ(std::string("simple_value"), Dotenv::get("NO_QUOTES"));
    
    Dotenv::clear();
    removeTestFile("test_quoted.env");
}

TEST(comments) {
    createTestEnvFile("test_comments.env",
        "# This is a comment\n"
        "KEY1=value1\n"
        "# Another comment\n"
        "KEY2=value2 # inline comment\n"
        "KEY3=\"value3\" # comment after quotes\n"
    );
    
    Dotenv::load("test_comments.env");
    ASSERT_EQ(std::string("value1"), Dotenv::get("KEY1"));
    ASSERT_EQ(std::string("value2"), Dotenv::get("KEY2"));
    ASSERT_EQ(std::string("value3"), Dotenv::get("KEY3"));
    
    Dotenv::clear();
    removeTestFile("test_comments.env");
}

TEST(whitespace_handling) {
    createTestEnvFile("test_whitespace.env",
        "  KEY1  =  value1  \n"
        "KEY2=   value2   \n"
        "KEY3  =value3\n"
    );
    
    Dotenv::load("test_whitespace.env");
    ASSERT_EQ(std::string("value1"), Dotenv::get("KEY1"));
    ASSERT_EQ(std::string("value2"), Dotenv::get("KEY2"));
    ASSERT_EQ(std::string("value3"), Dotenv::get("KEY3"));
    
    Dotenv::clear();
    removeTestFile("test_whitespace.env");
}

TEST(empty_lines) {
    createTestEnvFile("test_empty.env",
        "\n"
        "KEY1=value1\n"
        "\n"
        "\n"
        "KEY2=value2\n"
        "\n"
    );
    
    Dotenv::load("test_empty.env");
    ASSERT_EQ(std::string("value1"), Dotenv::get("KEY1"));
    ASSERT_EQ(std::string("value2"), Dotenv::get("KEY2"));
    
    Dotenv::clear();
    removeTestFile("test_empty.env");
}

TEST(escape_sequences) {
    createTestEnvFile("test_escape.env",
        "NEWLINE=\"line1\\nline2\"\n"
        "TAB=\"col1\\tcol2\"\n"
        "BACKSLASH=\"path\\\\to\\\\file\"\n"
    );
    
    Dotenv::load("test_escape.env");
    ASSERT_EQ(std::string("line1\nline2"), Dotenv::get("NEWLINE"));
    ASSERT_EQ(std::string("col1\tcol2"), Dotenv::get("TAB"));
    ASSERT_EQ(std::string("path\\to\\file"), Dotenv::get("BACKSLASH"));
    
    Dotenv::clear();
    removeTestFile("test_escape.env");
}

TEST(default_values) {
    createTestEnvFile("test_default.env",
        "EXISTING_KEY=existing_value\n"
    );
    
    Dotenv::load("test_default.env");
    ASSERT_EQ(std::string("existing_value"), Dotenv::get("EXISTING_KEY"));
    ASSERT_EQ(std::string("default_val"), Dotenv::get("NON_EXISTING_KEY", "default_val"));
    ASSERT_EQ(std::string(""), Dotenv::get("NON_EXISTING_KEY"));
    
    Dotenv::clear();
    removeTestFile("test_default.env");
}

TEST(has_function) {
    createTestEnvFile("test_has.env",
        "PRESENT_KEY=value\n"
    );
    
    Dotenv::load("test_has.env");
    ASSERT_TRUE(Dotenv::has("PRESENT_KEY"));
    ASSERT_FALSE(Dotenv::has("MISSING_KEY"));
    
    Dotenv::clear();
    removeTestFile("test_has.env");
}

TEST(is_loaded) {
    Dotenv::clear();
    ASSERT_FALSE(Dotenv::isLoaded());
    
    createTestEnvFile("test_loaded.env", "KEY=value\n");
    Dotenv::load("test_loaded.env");
    ASSERT_TRUE(Dotenv::isLoaded());
    
    Dotenv::clear();
    ASSERT_FALSE(Dotenv::isLoaded());
    
    removeTestFile("test_loaded.env");
}

TEST(file_not_found) {
    DotenvError result = Dotenv::load("nonexistent_file_12345.env");
    ASSERT_EQ(DotenvError::FileNotFound, result);
    ASSERT_FALSE(Dotenv::getLastError().empty());
}

TEST(loaded_keys) {
    Dotenv::clear();
    createTestEnvFile("test_keys.env",
        "KEY_A=value_a\n"
        "KEY_B=value_b\n"
        "KEY_C=value_c\n"
    );
    
    Dotenv::load("test_keys.env");
    auto keys = Dotenv::getLoadedKeys();
    ASSERT_EQ(3u, keys.size());
    
    Dotenv::clear();
    removeTestFile("test_keys.env");
}

TEST(c_api_load) {
    createTestEnvFile("test_c_api.env", "C_KEY=c_value\n");
    
    int result = DotenvLoad("test_c_api.env");
    ASSERT_EQ(0, result);
    
    const char* value = DotenvGet("C_KEY", nullptr);
    ASSERT_TRUE(value != nullptr);
    ASSERT_EQ(std::string("c_value"), std::string(value));
    
    DotenvClear();
    removeTestFile("test_c_api.env");
}

TEST(c_api_has) {
    createTestEnvFile("test_c_has.env", "C_HAS_KEY=present\n");
    
    DotenvLoad("test_c_has.env");
    ASSERT_TRUE(DotenvHas("C_HAS_KEY") == 1);
    ASSERT_TRUE(DotenvHas("MISSING_C_KEY") == 0);
    
    DotenvClear();
    removeTestFile("test_c_has.env");
}

TEST(legacy_api) {
    createTestEnvFile("test_legacy.env", "LEGACY_KEY=legacy_value\n");
    
    CallDotenvLoad("test_legacy.env");
    ASSERT_EQ(std::string("legacy_value"), Dotenv::get("LEGACY_KEY"));
    
    Dotenv::clear();
    removeTestFile("test_legacy.env");
}

TEST(special_characters_in_value) {
    createTestEnvFile("test_special.env",
        "URL=\"https://example.com?foo=bar&baz=qux\"\n"
        "REGEX=\"^[a-z]+$\"\n"
        "JSON='{\"key\": \"value\"}'\n"
    );
    
    Dotenv::load("test_special.env");
    ASSERT_EQ(std::string("https://example.com?foo=bar&baz=qux"), Dotenv::get("URL"));
    ASSERT_EQ(std::string("^[a-z]+$"), Dotenv::get("REGEX"));
    ASSERT_EQ(std::string("{\"key\": \"value\"}"), Dotenv::get("JSON"));
    
    Dotenv::clear();
    removeTestFile("test_special.env");
}

int main() {
    std::cout << "=== Dotenv C++ Library Tests ===" << std::endl << std::endl;
    
    RUN_TEST(basic_load);
    RUN_TEST(quoted_values);
    RUN_TEST(comments);
    RUN_TEST(whitespace_handling);
    RUN_TEST(empty_lines);
    RUN_TEST(escape_sequences);
    RUN_TEST(default_values);
    RUN_TEST(has_function);
    RUN_TEST(is_loaded);
    RUN_TEST(file_not_found);
    RUN_TEST(loaded_keys);
    RUN_TEST(c_api_load);
    RUN_TEST(c_api_has);
    RUN_TEST(legacy_api);
    RUN_TEST(special_characters_in_value);
    
    std::cout << std::endl;
    std::cout << "=== Results ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    
    return failed > 0 ? 1 : 0;
}
