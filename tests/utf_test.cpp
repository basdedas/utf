#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../inc/utf.h"
#include "../exc/NotFoundException"

namespace {

using Bytes = std::vector<uint8_t>;

Bytes to_bytes(const std::string& s) {
    return Bytes(s.begin(), s.end());
}

// Builds a utf object from text and returns what print() would show.
std::string content_of(utf& u) {
    testing::internal::CaptureStdout();
    u.print();
    return testing::internal::GetCapturedStdout();
}

// Writes text to a temporary file that is removed when the object goes out of scope.
class TempFile {
public:
    explicit TempFile(const std::string& text) {
        path_ = std::filesystem::temp_directory_path() /
                ("utf_test_" + std::to_string(counter_++) + ".txt");
        std::ofstream out(path_, std::ios::binary);
        out << text;
    }
    ~TempFile() { std::filesystem::remove(path_); }
    std::string path() const { return path_.string(); }

private:
    std::filesystem::path path_;
    static inline int counter_ = 0;
};

}  // namespace

// ---------------------------------------------------------------- parsing

TEST(Parsing, SplitsAsciiIntoSingleByteCharacters) {
    auto chars = utf::convert_chars_to_vector(to_bytes("abc"));
    ASSERT_EQ(chars.size(), 3u);
    for (const auto& c : chars) EXPECT_EQ(c.size(), 1u);
}

TEST(Parsing, RecognisesOneToFourByteCharacters) {
    // a (1 byte), e-acute (2), CJK (3), emoji (4)
    auto chars = utf::convert_chars_to_vector(to_bytes("a\xC3\xA9\xE4\xB8\xAD\xF0\x9F\x98\x80"));
    ASSERT_EQ(chars.size(), 4u);
    EXPECT_EQ(chars[0].size(), 1u);
    EXPECT_EQ(chars[1].size(), 2u);
    EXPECT_EQ(chars[2].size(), 3u);
    EXPECT_EQ(chars[3].size(), 4u);
}

TEST(Parsing, EmptyInputGivesNoCharacters) {
    EXPECT_TRUE(utf::convert_chars_to_vector({}).empty());
}

TEST(Parsing, RejectsStrayContinuationByte) {
    // 0x80 is a continuation byte and can never start a character.
    EXPECT_THROW(utf::convert_chars_to_vector(Bytes{0x80, 'a', 'b', 'c'}), std::runtime_error);
}

TEST(Parsing, RejectsTruncatedSequenceAtEnd) {
    // 0xE4 announces a 3-byte character but only one byte follows.
    EXPECT_THROW(utf::convert_chars_to_vector(Bytes{'a', 0xE4, 0xB8}), std::runtime_error);
}

TEST(Parsing, DropsCarriageReturns) {
    // Documents current behaviour: CR (byte 13) is skipped, so Windows line endings become LF.
    auto chars = utf::convert_chars_to_vector(to_bytes("a\r\nb"));
    EXPECT_EQ(chars.size(), 3u);
}

// ----------------------------------------------------------------- search

TEST(Search, ReturnsCharacterIndicesNotByteIndices) {
    utf u(to_bytes("a不会b不会"));
    // Each CJK character is 3 bytes, but indices count characters.
    EXPECT_EQ(u.search("不会"), (std::vector<size_t>{1, 4}));
}

TEST(Search, FindsAllOccurrences) {
    utf u(to_bytes("Never gonna, never gonna, Never"));
    EXPECT_EQ(u.search("Never"), (std::vector<size_t>{0, 26}));
}

TEST(Search, ThrowsNotFoundWhenOnlyFirstCharacterMatches) {
    utf u(to_bytes("hello"));
    EXPECT_THROW(u.search("help"), NotFoundException);
}

TEST(Search, PartialMatchAtEndOfTextDoesNotReadPastIt) {
    utf u(to_bytes("xa"));
    EXPECT_THROW(u.search("ab"), NotFoundException);
}

TEST(Search, ThrowsNotFoundWhenFirstCharacterIsAbsent) {
    utf u(to_bytes("hello"));
    EXPECT_THROW(u.search("xyz"), NotFoundException);
}

TEST(Search, EmptyStringIsRejected) {
    utf u(to_bytes("hello"));
    EXPECT_THROW(u.search(""), std::invalid_argument);
}

// ---------------------------------------------------------------- replace

TEST(Replace, SameLengthMultibyte) {
    utf u(to_bytes("我会你会"));
    u.replace("会", "不");
    EXPECT_EQ(content_of(u), "我不你不");
}

TEST(Replace, LongerReplacementKeepsLaterMatchesCorrect) {
    utf u(to_bytes("a-b-c-d"));
    u.replace("-", "--");
    EXPECT_EQ(content_of(u), "a--b--c--d");
}

TEST(Replace, ShorterReplacementKeepsLaterMatchesCorrect) {
    utf u(to_bytes("aXXbXXcXX"));
    u.replace("XX", "Y");
    EXPECT_EQ(content_of(u), "aYbYcY");
}

TEST(Replace, MultibyteWithAscii) {
    utf u(to_bytes("你不会放弃,不会"));
    u.replace("不会", "aaa");
    EXPECT_EQ(content_of(u), "你aaa放弃,aaa");
}

TEST(Replace, WithEmptyStringActsLikeDelete) {
    utf u(to_bytes("a-b-c"));
    u.replace("-", "");
    EXPECT_EQ(content_of(u), "abc");
}

TEST(Replace, OverlappingMatchesAreReplacedLeftToRight) {
    // "aa" matches at index 0 and 1 in "aaa"; only the first, non-overlapping match counts.
    utf u(to_bytes("aaa"));
    u.replace("aa", "b");
    EXPECT_EQ(content_of(u), "ba");
}

TEST(Replace, NotFoundThrowsNotFoundException) {
    utf u(to_bytes("hello"));
    EXPECT_THROW(u.replace("zzz", "y"), NotFoundException);
}

// ----------------------------------------------------------------- delete

TEST(Delete, OverlappingMatchesAreDeletedLeftToRight) {
    utf u(to_bytes("aaa"));
    u.delete_value("aa");
    EXPECT_EQ(content_of(u), "a");
}

TEST(Delete, NotFoundThrowsNotFoundException) {
    utf u(to_bytes("hello"));
    EXPECT_THROW(u.delete_value("zzz"), NotFoundException);
}

TEST(Delete, RemovesEveryOccurrence) {
    utf u(to_bytes("abxabyab"));
    u.delete_value("ab");
    EXPECT_EQ(content_of(u), "xy");
}

TEST(Delete, MultibyteCharacters) {
    utf u(to_bytes("一二三一二"));
    u.delete_value("一二");
    EXPECT_EQ(content_of(u), "三");
}

// ----------------------------------------------------------------- insert

TEST(Insert, AtStart) {
    utf u(to_bytes("xyz"));
    u.insert_value("Always", 0);
    EXPECT_EQ(content_of(u), "Alwaysxyz");
}

TEST(Insert, InTheMiddleCountsCharactersNotBytes) {
    utf u(to_bytes("你好吗"));
    u.insert_value("A", 2);  // after the second character
    EXPECT_EQ(content_of(u), "你好A吗");
}

TEST(Insert, AtEnd) {
    utf u(to_bytes("xyz"));
    u.insert_value("!", 3);
    EXPECT_EQ(content_of(u), "xyz!");
}

TEST(Insert, PastTheEndThrows) {
    utf u(to_bytes("xyz"));
    EXPECT_THROW(u.insert_value("!", 4), std::out_of_range);
}

// ------------------------------------------------------------- file input

TEST(FileInput, LoadsAndSearchesFile) {
    TempFile file("first line\n你知道规矩\nlast line\n");
    utf u(file.path());
    EXPECT_FALSE(u.search("规矩").empty());
}

TEST(FileInput, MissingFileThrows) {
    EXPECT_THROW(utf("/definitely/not/a/real/file.txt"), std::runtime_error);
}

TEST(FileInput, EndToEndLikeTheDemo) {
    // Mirrors main.cpp: search, replace, delete, insert on one file.
    TempFile file("Never say never. Never again.\n");
    utf u(file.path());

    EXPECT_EQ(u.search("Never"), (std::vector<size_t>{0, 17}));
    u.replace("Never", "Always");
    EXPECT_EQ(content_of(u), "Always say never. Always again.\n");

    u.delete_value("Always ");
    EXPECT_EQ(content_of(u), "say never. again.\n");

    u.insert_value("Always ", 0);
    EXPECT_EQ(content_of(u), "Always say never. again.\n");
}