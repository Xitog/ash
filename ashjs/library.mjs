//-----------------------------------------------------------------------------
// Function
//-----------------------------------------------------------------------------

// is t1 kindOf t2 ?
function kindOf(t1, t2) {
    return (
        (t2 === 'any')
        || (t2 === 'int' && t1 === 'nat') // int inclut nat
        || (t2 === 'num' && ['int', 'nat'].includes(t2)) // num inclut int & nat
        || (t1 === t2)
    );
}
//-----------------------------------------------------------------------------
// Classes
//-----------------------------------------------------------------------------

class NotAnExpression {
    toString() {
        return "Not an expression";
    }
}
const notAnExpression = new NotAnExpression();

class NilClass {
    toString() {
        return "nil";
    }
}
const nil = new NilClass();

class Value {
    constructor(identifier, type, value) {
        this.identifier = identifier;
        this.type = type;
        this.value = value;
    }

    setValue(value) {
        if (this.type !== Library.typeJStoAsh(value)) {
            throw new Error(`[ERROR] Variable ${this.identifier} is of type ${this.type} cannot set to ${value} of type ${Library.typeJStoAsh(value)}`);
        }
        this.value = value;
    }

    getValue() {
        return this.value;
    }

    getType() {
        return this.type;
    }
}

class BoundedFunction {
    constructor(o, f) {
        this.o = o;
        this.f = f;
    }
    do(args) {
        return this.f.apply(this.o, args);
    }
}

class AshObject {
    constructor(type = 'Object', value = null) {
        this.value = value;
        this.type = type;
    }
    toString() {
        return `|${this.type} ${this.value}|`;
    }
}

class List extends AshObject {
    constructor(value=[]) {
        super('List', value);
    }
    toString() {
        return `|${this.type} (${this.value.length}) ${this.value}|`;
    }
    first() {
        if (this.value.length === 0) {
            return nil;
        }
        return this.value[0];
    }
    head() {
        return this.first();
    }
    last() {
        if (this.value.length === 0) {
            return nil;
        }
        return this.value[this.value.length - 1];
    }
    at(index) {
        let res = null;
        if (Library.typeJStoAsh(index) === 'nat') {
            if (index - 1 < 0 || index - 1 >= this.value.length) {
                throw new Error(`[ERROR] Index ${index} out of bound: 1..${this.value.length}.`);
            }
            res = this.value[index - 1];
        } else if (Library.typeJStoAsh(index) === 'int') {
            res = this.value[this.value.length + index]; // negative
        } else {
            throw new Error(`[ERROR] An index must be a natural or an integer not a ${Library.typeJStoAsh(index)}.`);
        }
        return res;
    }
    add(lst) {
        if (!(lst instanceof List)) {
            throw new Error(`[ERROR] Only a list can be added to a list not a ${Library.typeJStoAsh(right)}.`);
        }
        this.value += lst;
    }
    max() {
        return Math.max(...this.value);
    }
    min() {
        return Math.min(...this.value);
    }
    includes(value) {
        return this.value.includes(value);
    }
    insert(pos, value) {
        this.value.insert(pos - 1, value);
    }
    push(value) {
        this.value.push(value);
    }
    append(value) {
        this.value.push(value);
    }
    concat(lst) {
        this.value += lst;
    }
    remove(pos) {
        this.value.splice(pos - 1, 1);
    }
    sort() {
        this.value.sort();
    }
    find(value) {
        return this.value.indexOf(value) + 1;
    }
    flatten() {
        let res = [];
        for (let e of this.value) {
            if (e instanceof List) {
                e.flatten();
                res = res.concat(e.value);
            } else {
                res.push(e);
            }
        }
        this.value = res;
    }
    pop() {
        return this.value.pop();
    }
    shift() {
        return this.value.shift();
    }
    unshift(value) {
        this.value.unshift(value);
    }
    prepend(value) {
        this.value.unshift(value);
    }
    size() {
        return this.value.length;
    }
    length() {
        return this.value.length;
    }
    count() {
        return this.value.length;
    }
    /*
    append
    head
    tail
    filter
    map
    [ for x in y return z ]
    [ x for x in y ]
    split
    include?
    in
    any?
    all?
    join(s)
    unique
    reverse
    zip
    reduce
    Map-Apply / Reduce / Filter
    freeze
    frozen?
    */
    method(msg) {
        if (!this.methods().includes(msg)) {
            throw new Error(`[ERROR] Unknown method ${msg} for type ${this.type}.`);
        }
        return this[msg];
    }
    methods() {
        return new List(['add', 'first', 'last', 'unshift', 'at', 'methods']);
    }
}

/*
dict
exist? / include? / key?
delete / remove
keys
values
merge, update
freeze
frozen?
*/

class Parameter {
    /**
     * Il y a une subtilité dans le defaultValue.
     * Si sa valeur est à "nil" à la création de l'instance (et pas null)
     * c'est qu'on autorise à ne pas donner de valeur à l'argument
     * @param {*} name
     * @param {*} type
     * @param {*} defaultValue
     */
    constructor(name, type, defaultValue = null) {
        this.name = name;
        this.type = type;
        this.defaultValue = defaultValue;
    }

    hasDefault() {
        return this.defaultValue !== null;
    }

    getDefault() {
        if (this.hasDefault()) {
            return this.defaultValue;
        }
        throw new Error(`Not default defined for parameter ${this}`);
    }

    getType() {
        return this.type;
    }

    toString() {
        let def = this.defaultValue !== null ? ` = ${this.defaultValue}` : "";
        return `${this.name} : ${this.type}${def}`;
    }
}

class Function extends Value {
    constructor(identifier, type, value, parameters, code) {
        // value is only "procedure" or "function"
        // type is its return type
        super(identifier, type, value);
        this.parameters = parameters;
        this.code = code;
    }

    toString() {
        return (
            `function ${this.identifier} (` +
            this.parameters.map((x) => x.toString()).join(", ") +
            ")"
        );
    }

    isProcedure() {
        return this.value === 'procedure';
    }

    call(args) {
        if (!Array.isArray(args)) {
            throw new Error(`Args should be a list, not ${typeof args}`);
        }
        for (let i = 0; i < this.parameters.length; i++) {
            // On a moins d'arguments que de paramètres
            if (i >= args.length) {
                for (let j = i; j < this.parameters.length; j++) {
                    if (!this.parameters[j].hasDefault()) {
                        throw new Error(`Missing parameter #${j}: ${this.parameters[j]}`);
                    }
                    args.push(this.parameters[j].getDefault());
                }
            } else if (!kindOf(Library.typeJStoAsh(args[i]), this.parameters[i].getType())) {
                throw new Error(`Wrong parameter type at #${i}: ${this.parameters[i]} expected vs ${args[i]} : ${Library.typeJStoAsh(args[i])}`);
            }
        }
        return this.code(args);
    }
}

class Library {
    static GlobalInterpreter;

    static init(globalInterpreter) {
        Library.GlobalInterpreter = globalInterpreter;
    }

    static sendMessage(idType, idFun, args) {
        if (idType === null) {
            if (idFun in table) {
                return table[idFun].call(args);
            }
        }
        throw new Error(`[ERROR] Unknown function ${idFun}`);
    }

    //-------------------------------------------------------------------------
    // Console functions
    //-------------------------------------------------------------------------

    static log (args) {
        Library.GlobalInterpreter.output_function(args.join(' '));
        return notAnExpression;
    }

    static read (args) {
        return Library.GlobalInterpreter.input_function('AAAA:');
    }

    //-------------------------------------------------------------------------
    // Helper functions
    //-------------------------------------------------------------------------

    static getTypeJS(value) {
        let typeValue = typeof value;
        let res = null;
        if (typeValue === 'object') {
            if (Array.isArray(value)) {
                res = 'Array';
            } else {
                res = value.constructor.name;
            }
        } else if (['boolean', 'string'].includes(typeValue)) {
            res = typeValue;
        } else if (typeValue === 'number') {
            if (Number.isInteger(value) && value >= 0) {
                res = 'number_natural';
            } else if (Number.isInteger(value)) {
                res = 'number_integer';
            } else {
                res = 'number_float';
            }
        } else {
            throw new Error(`[ERROR] Unknow JavaScript type for value |${value}|. typeof=${typeValue} isArray=${Array.isArray(value)}`);
        }
        return res;
    }

    static typeJStoAsh(value) {
        let typeJS = Library.getTypeJS(value);
        let equivalence = {
            'Array': 'list',
            'boolean': 'bool',
            'string': 'str',
            'number_integer': 'int',
            'number_natural': 'nat',
            'number_float': 'num'
        };
        return typeJS in equivalence ? equivalence[typeJS] : typeJS;
    }

    static produceDocumentation() {
        for (const [funID, funData] of Object.entries(table)) {
            let keyword = funData.isProcedure() ? 'procedure' : 'function';
            let retType = funData.isProcedure() ? '' : ' > ' + funData.getType();
            console.log(`${keyword} ${funID}(${funData.parameters.join(', ')}${retType})`);
        }
    }
    //-------------------------------------------------------------------------
    // Graphic functions
    //-------------------------------------------------------------------------

    static clear(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            context.clearRect(0, 0, 640, 480);
        }
        return notAnExpression;
    }

    static line(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            let x1 = args[0];
            let y1 = args[1];
            let x2 = args[2];
            let y2 = args[3];
            context.lineWidth = args[4];
            context.strokeStyle = args[5];
            context.beginPath();
            context.moveTo(x1, y1);
            context.lineTo(x2, y2);
            context.stroke();
        }
        return notAnExpression;
    }

    static circle(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            let centerX = args[0];
            let centerY = args[1];
            let radius = args[2];
            let color = args[3];
            let full = args[4];
            context.beginPath();
            context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
            if (full) {
                if (color !== nil) context.fillStyle = color;
                context.fill();
            } else {
                if (color !== nil) context.strokeStyle = color;
                context.stroke();
            }
        }
        return notAnExpression;
    }

    static rect(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            if (args[5]) {
                if (args[4] !== nil) context.fillStyle = args[4];
                context.fillRect(
                    args[0],
                    args[1],
                    args[2],
                    args[3]
                );
            } else {
                if (args[4] !== nil) context.strokeStyle = args[4];
                context.strokeRect(
                    args[0],
                    args[1],
                    args[2],
                    args[3]
                );
            }
        }
        return notAnExpression;
    }

    static draw(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            context.drawImage(args[2], args[0], args[1]);
        }
        return notAnExpression;
    }

    static text(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            ctx.fillText(args[0], args[1], args[2]);
        }
        return notAnExpression;
    }

    static setFont(args) {
        let context = Library.GlobalInterpreter.getContext();
        context.font = `${args[1]}px ${args[0]}`;
        return notAnExpression;
    }

    static setColor(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            context.fillStyle = args[0];
            context.strokeStyle = args[0];
        }
        return notAnExpression;
    }

    static getColor(args) {
        let context = Library.GlobalInterpreter.getContext();
        if (context !== null) {
            return context.strokeStyle;
        }
    }
}

//-----------------------------------------------------------------------------
// Function descriptions
//-----------------------------------------------------------------------------

const table = {
    'log': new Function(
        'log',
        nil,
        'procedure',
        [
            new Parameter('o', 'any')
        ],
        Library.log
    ),
    'read': new Function(
        'read',
        'str',
        'function',
        [],
        Library.read
    ),
    'clear': new Function(
        'clear',
        nil,
        'procedure',
        [],
        Library.clear
    ),
    'line': new Function(
        'line',
        nil,
        'procedure',
        [
            new Parameter('x1', 'nat'),
            new Parameter('y1', 'nat'),
            new Parameter('x2', 'nat'),
            new Parameter('y2', 'nat'),
            new Parameter('color', 'str', nil)
        ],
        Library.line
    ),
    'circle': new Function(
        'circle',
        nil,
        'procedure',
        [
            new Parameter('x', 'nat'),
            new Parameter('y', 'nat'),
            new Parameter('r', 'nat'),
            new Parameter('color', 'str', nil),
            new Parameter('fill', 'bool', false)
        ],
        Library.circle
    ),
    'rect': new Function(
        'rect',
        nil,
        'procedure',
        [
            new Parameter('x', 'nat'),
            new Parameter('y', 'nat'),
            new Parameter('width', 'nat'),
            new Parameter('height', 'nat'),
            new Parameter('color', 'str', nil),
            new Parameter('fill', 'bool', false)
        ],
        Library.rect
    ),
    'draw': new Function(
        'draw',
        nil,
        'procedure',
        [
            new Parameter('x', 'nat'),
            new Parameter('y', 'nat'),
            new Parameter('image', 'str')
        ],
        Library.draw
    ),
    'text': new Function(
        'text',
        nil,
        'procedure',
        [
            new Parameter('x', 'nat'),
            new Parameter('y', 'nat'),
            new Parameter('s', 'str')
        ],
        Library.text
    ),
    'set_font': new Function(
        'set_font',
        nil,
        'procedure',
        [
            new Parameter('police', 'str'),
            new Parameter('size', 'nat')
        ],
        Library.setFont
    ),
    'set_color': new Function(
        'set_color',
        nil,
        'procedure',
        [
            new Parameter('c', 'str')
        ],
        Library.setColor
    ),
    'get_color': new Function(
        'get_color',
        'str',
        'function',
        [],
        Library.getColor
    )
};

//-----------------------------------------------------------------------------
// Exports
//-----------------------------------------------------------------------------

export { Library, Function, Value, List, nil, notAnExpression, BoundedFunction };
