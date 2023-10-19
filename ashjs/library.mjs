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
            'number_int': 'int',
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

export { Library, Function, Value, nil, notAnExpression };
