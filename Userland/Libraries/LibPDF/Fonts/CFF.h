/*
 * Copyright (c) 2023, Rodrigo Tobar <rtobarc@gmail.com>.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Tuple.h>
#include <AK/Types.h>
#include <LibPDF/Error.h>
#include <LibPDF/Fonts/Type1FontProgram.h>

namespace PDF {

class Reader;

// CFF spec: https://adobe-type-tools.github.io/font-tech-notes/pdfs/5176.CFF.pdf

class CFF : public Type1FontProgram {

private:
    // CFF spec, "Table 9 Top DICT Operator Entries"
    enum class TopDictOperator {
        Version = 0,
        Notice,
        FullName,
        FamilyName,
        Weight,
        FontBBox,
        UniqueID = 13,
        XUID,
        Charset = 15,
        Encoding,
        CharStrings,
        Private,
        IsFixedPitch = (12 << 8 | 1),
        ItalicAngle,
        UnderlinePosition,
        UnderlineThickness,
        PaintType,
        CharstringType,
        FontMatrix,
        StrokeWidth,
        SyntheticBase = (12 << 8 | 20),
        PostScript,
        BaseFontName,
        BaseFontBlend,
    };

    // CFF spec, "Table 23 Private DICT Operators"
    enum class PrivDictOperator {
        Subrs = 19,
        DefaultWidthX,
        NominalWidthX,
    };

public:
    static PDFErrorOr<NonnullRefPtr<CFF>> create(ReadonlyBytes const&, RefPtr<Encoding> encoding);

    // to private
    using Card8 = u8;
    using Card16 = u16;
    using Offset = i32;
    using OffSize = u8;
    using SID = u16;
    using DictOperand = Variant<int, float>;

    static float to_number(DictOperand operand)
    {
        if (operand.has<int>())
            return operand.get<int>();
        return operand.get<float>();
    }

    static int load_int_dict_operand(u8 b0, Reader&);
    static float load_float_dict_operand(Reader&);
    static PDFErrorOr<DictOperand> load_dict_operand(u8, Reader&);

    using IndexDataHandler = Function<PDFErrorOr<void>(ReadonlyBytes const&)>;
    static PDFErrorOr<void> parse_index(Reader& reader, IndexDataHandler&&);

    template<typename OffsetType>
    static PDFErrorOr<void> parse_index_data(Card16 count, Reader& reader, IndexDataHandler&);

    template<typename OperatorT>
    using DictEntryHandler = Function<PDFErrorOr<void>(OperatorT, Vector<DictOperand> const&)>;

    template<typename OperatorT>
    static PDFErrorOr<void> parse_dict(Reader& reader, DictEntryHandler<OperatorT>&& handler);

    template<typename OperatorT>
    static PDFErrorOr<OperatorT> parse_dict_operator(u8, Reader&);

    static PDFErrorOr<Vector<StringView>> parse_strings(Reader&);

    static PDFErrorOr<Vector<CFF::Glyph>> parse_charstrings(Reader&&, Vector<ByteBuffer> const& subroutines);

    static DeprecatedFlyString resolve_sid(SID, Vector<StringView> const&);
    static PDFErrorOr<Vector<DeprecatedFlyString>> parse_charset(Reader&&, size_t, Vector<StringView> const&);
    static PDFErrorOr<Vector<u8>> parse_encoding(Reader&&);
};

}
