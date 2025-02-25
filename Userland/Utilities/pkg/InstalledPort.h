/*
 * Copyright (c) 2023, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/String.h>
#include <AK/StringView.h>
#include <AK/Types.h>

constexpr StringView ports_database = "/usr/Ports/installed.db"sv;

class InstalledPort {
public:
    enum class Type {
        Auto,
        Manual,
    };
    static Optional<Type> type_from_string(StringView);

    static ErrorOr<HashMap<String, InstalledPort>> read_ports_database();
    static ErrorOr<void> for_each_by_type(HashMap<String, InstalledPort>&, Type type, Function<ErrorOr<void>(InstalledPort const&)> callback);

    InstalledPort(String name, Type type, String version)
        : m_name(move(name))
        , m_type(type)
        , m_version(move(version))
    {
    }

    Type type() const { return m_type; }
    StringView type_as_string_view() const
    {
        if (m_type == Type::Auto)
            return "Automatic"sv;
        if (m_type == Type::Manual)
            return "Manual"sv;
        VERIFY_NOT_REACHED();
    }

    StringView name() const { return m_name.bytes_as_string_view(); }
    StringView version() const { return m_version.bytes_as_string_view(); }
    ReadonlySpan<String> dependencies() const { return m_dependencies.span(); }

private:
    String m_name;
    Type m_type;
    String m_version;
    Vector<String> m_dependencies;
};
