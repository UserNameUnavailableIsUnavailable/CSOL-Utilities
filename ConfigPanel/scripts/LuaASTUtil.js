export const AST_NODE_CHUNK = "Chunk";
export const AST_NODE_ASSIGNMENT_STATEMENT = "AssignmentStatement";
export const AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION = "TableConstructorExpression";
export const AST_NODE_TABLE_VALUE = "TableValue";
export const AST_NODE_TABLE_KEY_STRING = "TableKeyString";
export const AST_NODE_CALL_EXPRESSION = "CallExpression";
export const AST_NODE_CALL_STATEMENT = "CallStatement";
export const AST_NODE_REPEAT_STATEMENT = "RepeatStatement";
export const AST_NODE_WHILE_STATEMENT = "WhileStatement";
export const AST_NODE_FOR_NUMERIC_STATEMENT = "ForNumericStatement";
export const AST_NODE_FOR_GENERIC_STATEMENT = "ForGenericStatement";
export const AST_NODE_IF_STATEMENT = "IfStatement";
export const AST_NODE_IF_CLAUSE = "IfClause";
export const AST_NODE_ELSEIF_CLAUSE = "ElseifClause";
export const AST_NODE_ELSE_CLAUSE = "ElseClause";
export const AST_NODE_BREAK_STATEMENT = "BreakStatement";
export const AST_NODE_LOCAL_STATEMENT = "LocalStatement";
export const AST_NODE_IDENTIFIER = "Identifier";
export const AST_NODE_UNARY_EXPRESSION = "UnaryExpression";
export const AST_NODE_LOGICAL_EXPRESSION = "LogicalExpression";
export const AST_NODE_BINARY_EXPRESSION = "BinaryExpression";
export const AST_NODE_STRING_LITERAL = "StringLiteral";
export const AST_NODE_NUMERIC_LITERAL = "NumericLiteral";
export const AST_NODE_BOOLEAN_LITERAL = "BooleanLiteral";
export const AST_NODE_NIL_LITERAL = "NilLiteral";
export const AST_NODE_TABLE_CALL_EXPRESSION = "TableCallExpression";
export const AST_NODE_MEMBER_EXPRESSION = "MemberExpression";
export const AST_NODE_INDEX_EXPRESSION = "IndexExpression";
export const AST_NODE_FUNCTION_DECLARATION = "FunctionDeclaration";
export const AST_NODE_RETURN_STATEMENT = "ReturnStatement";
export const AST_NODE_GOTO_STATEMENT = "GotoStatement";
export const AST_NODE_LABEL_STATEMENT = "LabelStatement";
export const AST_NODE_DO_STATEMENT = "DoStatement";
class ASTTypeError extends Error {
    constructor(expected_type, got_ast) {
        super(`AST node of type ${expected_type} expected, but got ${got_ast.type}.\n` +
            `AST:\n` +
            `${JSON.stringify(got_ast)}`);
    }
}
var INDENT = '\t';
export function GetIndentUnit() { return INDENT; }
export function SetIndentUnit(c) { INDENT = c; }
export function GenerateLuaCodeFromAST(ast, indent_level = 0, first_line_indent = true) {
    if (!ast) {
        return '';
    }
    let ret;
    switch (ast.type) {
        case AST_NODE_CHUNK:
            ret = generate_chunk(ast, indent_level);
            break;
        case AST_NODE_ASSIGNMENT_STATEMENT:
            ret = generate_assignment_statement(ast, indent_level);
            break;
        case AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION:
            ret = generate_table_constructor_expression(ast, indent_level);
            break;
        case AST_NODE_TABLE_VALUE:
            ret = generate_table_value(ast, indent_level);
            break;
        case AST_NODE_TABLE_KEY_STRING:
            ret = generate_table_key_string(ast, indent_level);
            break;
        case AST_NODE_CALL_EXPRESSION:
            ret = generate_call_expression(ast, indent_level);
            break;
        case AST_NODE_CALL_STATEMENT:
            ret = generate_call_statement(ast, indent_level);
            break;
        case AST_NODE_REPEAT_STATEMENT:
            ret = generate_repeat_statement(ast, indent_level);
            break;
        case AST_NODE_WHILE_STATEMENT:
            ret = generate_while_statement(ast, indent_level);
            break;
        case AST_NODE_FOR_NUMERIC_STATEMENT:
            ret = generate_for_numeric_statement(ast, indent_level);
            break;
        case AST_NODE_FOR_GENERIC_STATEMENT:
            ret = generate_for_generic_statement(ast, indent_level);
            break;
        case AST_NODE_IF_STATEMENT:
            ret = generate_if_statement(ast, indent_level);
            break;
        /* AST_NODE_IF_CLAUSE, AST_NODE_ELSEIF_CLAUSE, AST_NODE_ELSE_CLAUSE 已经在 generate_if_statement 中一并处理 */
        case AST_NODE_IF_CLAUSE: throw Error(`Unexpected AST node: ${AST_NODE_IF_CLAUSE}.`);
        case AST_NODE_ELSEIF_CLAUSE: throw Error(`Unexpected AST node: ${AST_NODE_ELSEIF_CLAUSE}.`);
        case AST_NODE_ELSE_CLAUSE: throw Error(`Unexpected AST node: ${AST_NODE_ELSE_CLAUSE}.`);
        case AST_NODE_BREAK_STATEMENT:
            ret = generate_break_statement(ast, indent_level);
            break;
        case AST_NODE_LOCAL_STATEMENT:
            ret = generate_local_statement(ast, indent_level);
            break;
        case AST_NODE_IDENTIFIER:
            ret = generate_identifier(ast, indent_level);
            break;
        case AST_NODE_UNARY_EXPRESSION:
            ret = generate_unary_expression(ast, indent_level);
            break;
        case AST_NODE_LOGICAL_EXPRESSION:
            ret = generate_logical_expression(ast, indent_level);
            break;
        case AST_NODE_BINARY_EXPRESSION:
            ret = generate_binary_expression(ast, indent_level);
            break;
        case AST_NODE_STRING_LITERAL:
            ret = generate_string_literal(ast, indent_level);
            break;
        case AST_NODE_NUMERIC_LITERAL:
            ret = generate_numeric_literal(ast, indent_level);
            break;
        case AST_NODE_BOOLEAN_LITERAL:
            ret = generate_boolean_literal(ast, indent_level);
            break;
        case AST_NODE_NIL_LITERAL:
            ret = generate_nil_literal(ast, indent_level);
            break;
        case AST_NODE_TABLE_CALL_EXPRESSION:
            ret = generate_table_call_expression(ast, indent_level);
            break;
        case AST_NODE_MEMBER_EXPRESSION:
            ret = generate_member_expression(ast, indent_level);
            break;
        case AST_NODE_INDEX_EXPRESSION:
            ret = generate_index_expression(ast, indent_level);
            break;
        case AST_NODE_FUNCTION_DECLARATION:
            ret = generate_function_declaration(ast, indent_level);
            break;
        case AST_NODE_RETURN_STATEMENT:
            ret = generate_return_statement(ast, indent_level);
            break;
        case AST_NODE_GOTO_STATEMENT:
            ret = generate_goto_statement(ast, indent_level);
            break;
        case AST_NODE_LABEL_STATEMENT:
            ret = generate_label_statement(ast, indent_level);
            break;
        case AST_NODE_DO_STATEMENT:
            ret = generate_do_statement(ast, indent_level);
            break;
        default: throw Error(`${ast.type} not handled. AST:\n${JSON.stringify(ast)}`);
    }
    if (!first_line_indent) {
        let pattern = new RegExp(`^(${INDENT})+`);
        ret = ret.replace(pattern, '');
    }
    return ret;
}
function generate_chunk(ast, indent_level) {
    if (ast.type != AST_NODE_CHUNK) {
        throw new ASTTypeError(AST_NODE_CHUNK, ast);
    }
    var indent = INDENT.repeat(indent_level);
    var chunk = "";
    ast.body.forEach(element => {
        chunk += GenerateLuaCodeFromAST(element, indent_level) + '\n';
    });
    return chunk;
}
function generate_assignment_statement(ast, indent_level) {
    if (ast.type != AST_NODE_ASSIGNMENT_STATEMENT) {
        throw new ASTTypeError(AST_NODE_ASSIGNMENT_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${ast.variables.map(_ast => indent + GenerateLuaCodeFromAST(_ast, indent_level))} = ${ast.init.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level, false))}`;
}
function generate_table_constructor_expression(ast, indent_level) {
    if (ast.type != AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION)
        throw new ASTTypeError(AST_NODE_TABLE_CONSTRUCTOR_EXPRESSION, ast);
    return `{\n` +
        `${ast.fields.map(_ast => INDENT.repeat(indent_level + 1) + GenerateLuaCodeFromAST(_ast, indent_level + 1)).join(",\n")}\n` +
        `${INDENT.repeat(indent_level)}}`;
}
function generate_table_value(ast, indent_level) {
    if (ast.type != AST_NODE_TABLE_VALUE) {
        throw new ASTTypeError(AST_NODE_TABLE_VALUE, ast);
    }
    return GenerateLuaCodeFromAST(ast.value, indent_level);
}
function generate_table_key_string(ast, indent_level) {
    if (ast.type != AST_NODE_TABLE_KEY_STRING) {
        throw new ASTTypeError(AST_NODE_TABLE_KEY_STRING, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.key, indent_level)} = ${GenerateLuaCodeFromAST(ast.value, indent_level, false)}`;
}
function generate_call_expression(ast, indent_level) {
    if (ast.type != AST_NODE_CALL_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_CALL_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.base, indent_level)}(${ast.arguments.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level, false)).join(', ')})`;
}
function generate_call_statement(ast, indent_level) {
    if (ast.type != AST_NODE_CALL_STATEMENT) {
        throw new ASTTypeError(AST_NODE_CALL_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return indent + GenerateLuaCodeFromAST(ast.expression, indent_level);
}
function generate_repeat_statement(ast, indent_level) {
    if (ast.type != AST_NODE_REPEAT_STATEMENT) {
        throw new ASTTypeError(AST_NODE_REPEAT_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}repeat` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n` +
        `${indent}until ${GenerateLuaCodeFromAST(ast.condition, indent_level)}`;
}
function generate_while_statement(ast, indent_level) {
    if (ast.type != AST_NODE_WHILE_STATEMENT) {
        throw new ASTTypeError(AST_NODE_WHILE_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}while ${GenerateLuaCodeFromAST(ast.condition, indent_level)}\n` +
        `${indent}do\n` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n` +
        `${indent}end`;
}
function generate_for_numeric_statement(ast, indent_level) {
    if (ast.type != AST_NODE_FOR_NUMERIC_STATEMENT) {
        throw new ASTTypeError(AST_NODE_FOR_NUMERIC_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}for ${GenerateLuaCodeFromAST(ast.variable, indent_level)} = ${GenerateLuaCodeFromAST(ast.start, indent_level)}, ${GenerateLuaCodeFromAST(ast.end, indent_level)}\n` +
        `${indent}do\n` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n` +
        `${indent}end`;
}
function generate_for_generic_statement(ast, indent_level) {
    if (ast.type != AST_NODE_FOR_GENERIC_STATEMENT) {
        throw new ASTTypeError(AST_NODE_FOR_GENERIC_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}for ${ast.variables.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level)).join(", ")} in ${ast.iterations.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join(", ")}\n` +
        `${indent}do\n` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level)).join('\n')}\n` +
        `${indent}end`;
}
function generate_if_statement(ast, indent_level) {
    if (ast.type != AST_NODE_IF_STATEMENT) {
        throw new ASTTypeError(AST_NODE_IF_STATEMENT, ast);
    }
    var block = "";
    var indent = INDENT.repeat(indent_level);
    ast.clauses.forEach(branch => {
        if (branch.type == AST_NODE_IF_CLAUSE) {
            block += `${indent}if ${GenerateLuaCodeFromAST(branch.condition)}\n` +
                `${indent}then\n` +
                `${branch.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n`;
        }
        else if (branch.type == AST_NODE_ELSEIF_CLAUSE) {
            block += `${indent}elseif ${GenerateLuaCodeFromAST(branch.condition)}\n` +
                `${indent}then\n` +
                `${branch.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n`;
        }
        else if (branch.type == AST_NODE_ELSE_CLAUSE) {
            block += `${indent}else\n` +
                `${branch.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n`;
        }
        else {
            throw new ASTTypeError(`${AST_NODE_IF_CLAUSE}, ${AST_NODE_ELSEIF_CLAUSE}, ${AST_NODE_ELSE_CLAUSE}`, branch);
        }
    });
    block += `${indent}end`;
    return block;
}
function generate_break_statement(ast, indent_level) {
    if (ast.type != AST_NODE_BREAK_STATEMENT) {
        throw new ASTTypeError(AST_NODE_BREAK_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}break`;
}
function generate_local_statement(ast, indent_level) {
    if (ast.type != AST_NODE_LOCAL_STATEMENT) {
        throw new ASTTypeError(AST_NODE_LOCAL_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}local ${ast.variables.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level)).join(", ")} = ${ast.init.map(GenerateLuaCodeFromAST).join(", ")}`;
}
function generate_identifier(ast, indent_level) {
    if (ast.type != AST_NODE_IDENTIFIER) {
        throw new ASTTypeError(AST_NODE_IDENTIFIER, ast);
    }
    return ast.name;
}
function generate_unary_expression(ast, indent_level) {
    if (ast.type != AST_NODE_UNARY_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_UNARY_EXPRESSION, ast);
    }
    return `${ast.operator} ${GenerateLuaCodeFromAST(ast.argument, indent_level)}`;
}
function generate_logical_expression(ast, indent_level) {
    if (ast.type != AST_NODE_LOGICAL_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_LOGICAL_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.left, indent_level)} ${ast.operator} ${GenerateLuaCodeFromAST(ast.right, indent_level)}`;
}
function generate_binary_expression(ast, indent_level) {
    if (ast.type != AST_NODE_BINARY_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_BINARY_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.left, indent_level)} ${ast.operator} ${GenerateLuaCodeFromAST(ast.right, indent_level)}`;
}
function generate_string_literal(ast, indent_level) {
    if (ast.type != AST_NODE_STRING_LITERAL) {
        throw new ASTTypeError(AST_NODE_STRING_LITERAL, ast);
    }
    return ast.raw;
}
function generate_numeric_literal(ast, indent_level) {
    if (ast.type != AST_NODE_NUMERIC_LITERAL) {
        throw new ASTTypeError(AST_NODE_NUMERIC_LITERAL, ast);
    }
    return ast.raw;
}
function generate_boolean_literal(ast, indent_level) {
    if (ast.type != AST_NODE_BOOLEAN_LITERAL) {
        throw new ASTTypeError(AST_NODE_BOOLEAN_LITERAL, ast);
    }
    return ast.raw;
}
function generate_nil_literal(ast, indent_level) {
    if (ast.type != AST_NODE_NIL_LITERAL) {
        throw new ASTTypeError(AST_NODE_NIL_LITERAL, ast);
    }
    return 'nil';
}
function generate_table_call_expression(ast, indent_level) {
    if (ast.type != AST_NODE_TABLE_CALL_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_TABLE_CALL_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.base, indent_level)}${GenerateLuaCodeFromAST(ast.arguments, indent_level)}`;
}
function generate_member_expression(ast, indent_level) {
    if (ast.type != AST_NODE_MEMBER_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_MEMBER_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.base, indent_level)}${ast.indexer}${GenerateLuaCodeFromAST(ast.identifier, indent_level)}`;
}
function generate_index_expression(ast, indent_level) {
    if (ast.type != AST_NODE_INDEX_EXPRESSION) {
        throw new ASTTypeError(AST_NODE_INDEX_EXPRESSION, ast);
    }
    return `${GenerateLuaCodeFromAST(ast.base, indent_level)}[${GenerateLuaCodeFromAST(ast.index, indent_level)}]`;
}
function generate_function_declaration(ast, indent_level) {
    if (ast.type != AST_NODE_FUNCTION_DECLARATION) {
        throw new ASTTypeError(AST_NODE_FUNCTION_DECLARATION, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}${ast.isLocal ? 'local ' : ''}function ${GenerateLuaCodeFromAST(ast.identifier)}(${ast.parameters.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level)).join(", ")})\n` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n` +
        `${indent}end`;
}
function generate_return_statement(ast, indent_level) {
    if (ast.type != AST_NODE_RETURN_STATEMENT) {
        throw new ASTTypeError(AST_NODE_RETURN_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}return ${ast.arguments.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level)).join(", ")}`;
}
function generate_goto_statement(ast, indent_level) {
    if (ast.type != AST_NODE_GOTO_STATEMENT) {
        throw new ASTTypeError(AST_NODE_GOTO_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}goto ${GenerateLuaCodeFromAST(ast.label, indent_level)}`;
}
function generate_label_statement(ast, indent_level) {
    if (ast.type != AST_NODE_LABEL_STATEMENT) {
        throw new ASTTypeError(AST_NODE_LABEL_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}::${GenerateLuaCodeFromAST(ast.label, indent_level)}::`;
}
function generate_do_statement(ast, indent_level) {
    if (ast.type != AST_NODE_DO_STATEMENT) {
        throw new ASTTypeError(AST_NODE_DO_STATEMENT, ast);
    }
    var indent = INDENT.repeat(indent_level);
    return `${indent}do\n` +
        `${ast.body.map(_ast => GenerateLuaCodeFromAST(_ast, indent_level + 1)).join('\n')}\n` +
        `${indent}end`;
}
