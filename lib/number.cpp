#include "number.h"
#include <cstring>
#include <cmath>
#include <cinttypes>


uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t number_239{};
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);

    while (value > 0) {
        uint32_t number = value % 128;
        PutNumber(number_239, number, byte, position);
        value /= 128;
    }

    WriteShift(number_239, shift);

    return number_239;
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint32_t len = strlen(str);
    if (len > 72) {
        std::cerr << "You entered a number greater than uint239_t" << std::endl;
        exit(1);
    }

    char str_del[len+1];
    for (int j = 0; j < len; j++) {
        str_del[j] = str[j];
    }
    str_del[len] = '\0';

    uint239_t number_239{};
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);
    
    uint32_t count_in_uint = 0;
    uint32_t remainder = 0;
    char new_str[len + 1];
    while (len > 0) {
        uint32_t remainder = 0;
        uint32_t count_in_nstr = 0;
        uint32_t insig_zero = 0;
        bool start_number = false;
        uint32_t i = 0;
        while (str_del[i] != '\0') {
            uint32_t result = (remainder * 10) + (str_del[i] - 48);
            if (result / 128 > 0) {
                new_str[count_in_nstr] = (result / 128) + 48;
                count_in_nstr++;
                remainder = result % 128;
                start_number = true;
            } else {
                new_str[count_in_nstr] = 48;
                count_in_nstr++;
                remainder = result;
                if (!start_number) {
                    insig_zero++;
                }
            }
            i++;
        }

        for (int j = insig_zero; j < count_in_nstr; j++) {
            str_del[j - insig_zero] = new_str[j];
        }
        len = count_in_nstr - insig_zero;
        str_del[len] = '\0';
        count_in_uint++;

        if (count_in_uint > 35 || (count_in_uint == 35 && remainder > 1)) {
            std::cerr << "You entered a number greater than uint239_t" << std::endl;
            exit(1);
        }

        PutNumber(number_239, remainder, byte, position);
    }

    WriteShift(number_239, shift);

    return number_239;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t number_239{};

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t shift = (lhs_shift + rhs_shift) % (uint64_t)pow(2, 35);
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);
    
    uint239_t lhs_wt_shift = MinusShift(lhs, lhs_shift);
    uint239_t rhs_wt_shift = MinusShift(rhs, rhs_shift);

    uint32_t remainder = 0;
    for (int i = 0; i < 35; i++) {
        remainder += lhs_wt_shift.data[34 - i] + rhs_wt_shift.data[34 - i];
        if (i == 34 && remainder > 1) {
            std::cerr << "Sum of number's greater than uint239_t" << std::endl;
            exit(1);
        }
        
        PutNumber(number_239, remainder % 128, byte, position);
        remainder /= 128;
    }

    WriteShift(number_239, shift);

    return number_239;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t number_239{};

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    int64_t kLimit_shift = pow(2, 35);
    int64_t shift = (kLimit_shift + (lhs_shift - rhs_shift)) % kLimit_shift;
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);
    
    uint239_t lhs_wt_shift = MinusShift(lhs, lhs_shift);
    uint239_t rhs_wt_shift = MinusShift(rhs, rhs_shift);
    
    bool remainder = false; 
    for (int i = 0; i < 35; i++) {
        int32_t minuend = lhs_wt_shift.data[34 - i];
        int32_t subtrahend = rhs_wt_shift.data[34 - i];
        if ((minuend - remainder) >= subtrahend) {
            uint32_t difference = minuend - remainder - subtrahend;
            PutNumber(number_239, difference, byte, position);
            remainder = false;
        } else {
            uint32_t difference = 128 + minuend - remainder - subtrahend;
            PutNumber(number_239, difference, byte, position);
            remainder = true;
        }
    }
    if (remainder) {
        std::cerr << "minuend biger than subtrahend but you use uint239_t";
        exit(1);
    }

    WriteShift(number_239, shift);

    return number_239;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t number_239{};

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    uint64_t shift = (lhs_shift + rhs_shift) % (uint64_t)pow(2, 35);
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);
    
    uint239_t lhs_wt_shift = MinusShift(lhs, lhs_shift);
    uint239_t rhs_wt_shift = MinusShift(rhs, rhs_shift);

    uint64_t remainder = 0;
    for (int i = 0; i < 35; i++){
        for (int j = 0; j < i + 1; j++) {
            remainder += lhs_wt_shift.data[34 - (i-j)] * rhs_wt_shift.data[34 - j];
        }
        if (i == 34 && remainder > 1) {
            std::cerr << "Sum of number's greater than uint239_t" << std::endl;
            exit(1);
        }

        PutNumber(number_239, remainder % 128, byte, position);
        remainder /= 128;
    }

    WriteShift(number_239, shift);

    return number_239;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t number_239{};

    uint64_t lhs_shift = GetShift(lhs);
    uint64_t rhs_shift = GetShift(rhs);
    int64_t kLimit_shift = pow(2, 35);
    int64_t shift = (kLimit_shift + (lhs_shift - rhs_shift)) % kLimit_shift;
    uint32_t byte = shift / 7;
    uint32_t position = pow(2, shift % 7);
    
    uint239_t lhs_wt_shift = MinusShift(lhs, lhs_shift);
    uint239_t rhs_wt_shift = MinusShift(rhs, rhs_shift);

    uint32_t count_bytes_lhs = OccupiedBytes(lhs_wt_shift);
    uint32_t count_bytes_rhs = OccupiedBytes(rhs_wt_shift);
    uint32_t count_bytes_answ = count_bytes_lhs - count_bytes_rhs;
    byte += count_bytes_answ;



    if (count_bytes_rhs == 0) {
        std::cerr << "You can't divide by zero" << std::endl;
        exit(1);
    }

    if (count_bytes_rhs > count_bytes_lhs) {
        return number_239;
    }

    uint239_t numeral{};
    for (int i = 0; i < count_bytes_rhs; i++) {
        numeral.data[35 - (count_bytes_rhs - i)] = lhs_wt_shift.data[35 - count_bytes_lhs];
        count_bytes_lhs--;
    }

    for (int i = 0; i <= count_bytes_answ; i++){
        if (numeral < rhs_wt_shift) {
            number_239.data[34 - (count_bytes_answ - i)] = 0;
        } else {
            uint32_t s = 0;
            uint32_t f = 128;
            uint239_t remainder{};
            while ((f - s) > 1) {
                uint32_t mid = (f + s) / 2;
                uint239_t prov = rhs_wt_shift * FromInt(mid, 0);
        
                if (prov > numeral) {
                    f = mid;
                } else {
                    remainder = prov;
                    s = mid;   
                }
            }
            byte -= i;
            PutNumber(number_239, s, byte, position);
            byte += i - 1;
            numeral = numeral - remainder;
        }

        if (count_bytes_lhs > 0) {
            for (int j = 0; j < 34; j++) {
                numeral.data[j] = numeral.data[j + 1];
            } 
            numeral.data[34] = lhs_wt_shift.data[35 - count_bytes_lhs];
            count_bytes_lhs--;
        }
    }

    WriteShift(number_239, shift);

    return number_239;
}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return true;
        }
    }
    return false;
}

bool operator>(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return lhs_wt_shift.data[i] > rhs_wt_shift.data[i];
        }
    }
    return false;
}

bool operator>=(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return lhs_wt_shift.data[i] > rhs_wt_shift.data[i];
        }
    }
    return true;
}

bool operator<(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return lhs_wt_shift.data[i] < rhs_wt_shift.data[i];
        }
    }
    return false;
}

bool operator<=(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t lhs_wt_shift = MinusShift(lhs, GetShift(lhs));
    uint239_t rhs_wt_shift = MinusShift(rhs, GetShift(rhs));
    for (int i = 0; i < 35; i++) {
        if (lhs_wt_shift.data[i] != rhs_wt_shift.data[i]) {
            return lhs_wt_shift.data[i] < rhs_wt_shift.data[i];
        }
    }
    return true;
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    uint239_t number_239 = MinusShift(value, GetShift(value));

    char result[72];
    uint32_t position = 0;
    bool number_is_zero = false;
    while (!number_is_zero) {
        number_is_zero = true;
        uint32_t remainder = 0;
        for (int i = 0; i < 35; i++) {
            remainder = remainder * 128 + number_239.data[i];
            number_239.data[i] = remainder / 10;
            remainder %= 10;
            if (number_239.data[i] != 0) {
                number_is_zero = false;
            }
        }

        result[position] = remainder + '0';
        position++;
    }

    for (int i = position - 1; i >= 0; i--) {
        stream << result[i];
    }

    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    uint64_t result = 0;

    for (int i = 0; i < 35; i++){
        if (int(value.data[i] & 128)) {
            result += pow(2, 34 - i);
        }
    }

    return result;
}

uint239_t MinusShift(const uint239_t& value, uint64_t shift) {
    uint239_t number_239{};
    uint32_t byte = shift / 7;
    uint32_t position_in_bin = pow(2, shift % 7);
    uint32_t rem_position = pow(2, 7 - (shift % 7));

    uint32_t number = 0;
    for (int i = 0; i < 35; i++) {
        number = value.data[34 - ((byte + i) % 35)] % 128 / position_in_bin;
        if (position_in_bin > 0)
        number += (value.data[34 - ((byte + 1 + i) % 35)] % position_in_bin) * rem_position;
        number_239.data[34 - i] = number;
    }

    return number_239;
}

void WriteShift(uint239_t& value, uint64_t shift) {
    uint239_t number_239;

    uint32_t i = 0;
    while (shift > 0) {
        value.data[34-i] += (shift % 2) * 128;
        shift /= 2;
        i++;
    }
}

void PutNumber(uint239_t& value, uint32_t number, uint32_t& byte, uint32_t position) {
    uint32_t number_with_shift = number * position;
    value.data[34 - (byte % 35)] += number_with_shift % 128;
    value.data[34 - ((byte + 1) % 35)] += number_with_shift / 128;
    byte++;
}

uint32_t OccupiedBytes(const uint239_t& value) {
    uint32_t result = 35;
    for (int i = 0; i < 35; i++) {
        if (value.data[i] != 0) {
            break;
        }
        result--;
    }
    return result;
}