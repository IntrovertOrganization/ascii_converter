#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>

enum class NumberSystem {
    BINARY = 2,
    OCTAL = 8,
    HEXADECIMAL = 16
};

struct ConversionResult {
    std::string value;
    std::vector<std::string> steps;
    NumberSystem base;
};

struct DecimalConversion {
    int result;
    std::string expression;
    std::string contributions;
};

class NumberConverter {
private:
    static const int BYTE_SIZE;
    static const std::string HEX_DIGITS;

public:
    static ConversionResult decimalToBase(int decimal, NumberSystem base) {
        if (decimal == 0) {
            return ConversionResult{"0", {"0"}, base};
        }

        std::string result;
        std::vector<std::string> steps;
        int number = decimal;

        while (number > 0) {
            int remainder = number % static_cast<int>(base);
            char digit = (base == NumberSystem::HEXADECIMAL) ? 
                        HEX_DIGITS[remainder] : std::to_string(remainder)[0];
            
            std::stringstream step;
            step << number << " : " << static_cast<int>(base) << " = " 
                 << (number / static_cast<int>(base)) << " // " << digit;
            steps.push_back(step.str());
            
            result = digit + result;
            number /= static_cast<int>(base);
        }

        int minPadding;
        switch (base) {
            case NumberSystem::BINARY: 
                minPadding = std::max(BYTE_SIZE, static_cast<int>(result.length())); break;
            case NumberSystem::OCTAL: 
                minPadding = std::max(3, static_cast<int>(result.length())); break;
            case NumberSystem::HEXADECIMAL: 
                minPadding = std::max(2, static_cast<int>(result.length())); break;
        }

        result = std::string(minPadding - result.length(), '0') + result;
        return ConversionResult{result, steps, base};
    }

    static DecimalConversion baseToDecimal(const std::string& value, NumberSystem base) {
        int decimal = 0;
        std::vector<int> contributions;
        std::stringstream expr;
        
        for (size_t i = 0; i < value.length(); i++) {
            int digitValue = (base == NumberSystem::HEXADECIMAL) ? 
                           getHexValue(value[i]) : value[i] - '0';
            int power = value.length() - 1 - i;
            int contribution = digitValue * std::pow(static_cast<int>(base), power);
            
            if (i > 0) expr << " + ";
            expr << "(" << digitValue << " * " << static_cast<int>(base) 
                 << "^" << power << ")";
            
            contributions.push_back(contribution);
            decimal += contribution;
        }

        std::stringstream contribStr;
        for (size_t i = 0; i < contributions.size(); i++) {
            if (i > 0) contribStr << " + ";
            contribStr << contributions[i];
        }

        return DecimalConversion{decimal, expr.str(), contribStr.str()};
    }

private:
    static int getHexValue(char hex) {
        if (hex >= '0' && hex <= '9') return hex - '0';
        if (hex >= 'A' && hex <= 'F') return 10 + (hex - 'A');
        if (hex >= 'a' && hex <= 'f') return 10 + (hex - 'a');
        return 0;
    }
};

const int NumberConverter::BYTE_SIZE = 8;
const std::string NumberConverter::HEX_DIGITS = "0123456789ABCDEF";

class ASCIIConverter {
private:
    std::string text;

public:
    ASCIIConverter(const std::string& input) : text(input) {}

    void process() {
        std::vector<std::string> decimalValues, binaryValues, octalValues, hexValues;

        for (char c : text) {
            int asciiValue = static_cast<int>(c);
            if (asciiValue > 255) {
                std::cout << "Warning: Character '" << c << "' is not in ASCII range\n";
                continue;
            }

            decimalValues.push_back(std::to_string(asciiValue));
            
            auto binConv = NumberConverter::decimalToBase(asciiValue, NumberSystem::BINARY);
            auto octConv = NumberConverter::decimalToBase(asciiValue, NumberSystem::OCTAL);
            auto hexConv = NumberConverter::decimalToBase(asciiValue, NumberSystem::HEXADECIMAL);

            binaryValues.push_back(binConv.value);
            octalValues.push_back(octConv.value);
            hexValues.push_back(hexConv.value);

            printConversions(c, asciiValue, binConv, octConv, hexConv);
        }

        printFinalResults(decimalValues, binaryValues, octalValues, hexValues);
    }

private:
    void printConversions(char c, int asciiValue, 
                         const ConversionResult& bin, 
                         const ConversionResult& oct, 
                         const ConversionResult& hex) {
        std::cout << "----------------------------------------------------------------------\n";
        
        // Print conversion steps for each base
        printBaseConversion("Binary", bin, c, asciiValue);
        printBaseConversion("Octal", oct, c, asciiValue);
        printBaseConversion("Hexadecimal", hex, c, asciiValue);
    }

    void printBaseConversion(const std::string& baseName, 
                           const ConversionResult& conv, 
                           char c, int asciiValue) {
        std::cout << "Formula to convert Decimal " << asciiValue 
                 << " (" << c << ") to " << baseName << ":\n";
        
        for (const auto& step : conv.steps) {
            std::cout << step << "\n";
        }
        std::cout << "\n";

        auto result = NumberConverter::baseToDecimal(conv.value, conv.base);
        std::cout << "Formula to convert " << baseName << " " << conv.value 
                 << " (" << c << ") back to Decimal:\n";
        std::cout << conv.value << " = " << result.expression << "\n";
        std::cout << "         = " << result.contributions << "\n";
        std::cout << "         = " << result.result << " (" 
                 << static_cast<char>(result.result) << ")\n\n";
    }

    void printFinalResults(const std::vector<std::string>& decimal,
                         const std::vector<std::string>& binary,
                         const std::vector<std::string>& octal,
                         const std::vector<std::string>& hex) {
        std::cout << "\nFinal Results:\n";
        printValueSet("Decimal", decimal);
        printValueSet("Binary", binary);
        printValueSet("Octal", octal);
        printValueSet("Hexadecimal", hex);
    }

    void printValueSet(const std::string& label, const std::vector<std::string>& values) {
        std::cout << label << ": ";
        for (size_t i = 0; i < values.size(); i++) {
            char c = text[i];
            std::cout << values[i];
            if (c >= 32 && c <= 126) {
                std::cout << " (" << c << ")";
            } else {
                std::cout << " (non-printable)";
            }
            std::cout << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    std::string text;
    do {
        std::cout << "Enter ASCII: ";
        std::getline(std::cin, text);
        if (text.empty()) {
            std::cout << "Error: Input cannot be empty!\n";
        }
    } while (text.empty());

    std::cout << "\nConverting ASCII '" << text 
              << "' to Decimal, Binary, Octal, and Hexadecimal!\n\n";

    ASCIIConverter converter(text);
    converter.process();

    return 0;
}