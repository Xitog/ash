def read_tests(filepath):
    global GLOBAL_DEBUG
    GLOBAL_DEBUG = True
    print('-------------------------------')
    type2python = {
        'Boolean' : bool,
        'Integer' : int,
        'String' : str
    }
    res2python = {
        'false' : False,
        'true' : True
    }
    f = open(filepath, mode='r')
    lines = f.readlines()
    f.close()
    nb_tests = {
        'skipped' : 0,
        'success' : 0,
        'failed'  : 0,
        'total'   : 0,
    }
    for i, line in enumerate(lines, start=1):
        if i < 3: continue # headers
        try:
            data    = line.split('\t')
            idt     = data[0]
            status  = data[1]
            py      = data[2]
            lua     = data[3]
            title   = data[4]
            content = data[5]
            content_exec = content.replace('\\n', '\n')
            resval  = data[6]
            restyp  = data[7]
            try:
                numtok  = int(data[8].rstrip())
            except ValueError:
                numtok = None
            asserts  = data[9:]
            if len(asserts) > 0:
                asserts[-1] = asserts[-1].rstrip()
        except (ValueError, IndexError) as e:
            print(f'[TEST]    {nb_tests["total"]+1:03} Skipping line {i:05d} {e}')
            nb_tests['skipped'] += 1
            nb_tests['total']   += 1
            continue
        if status == 'Do':
            print(f'[TEST]    {nb_tests["total"]+1:03} {title} : {content}')
            try:
                # Tokenizing
                res = Tokenizer(False).tokenize(content_exec)
                if numtok is not None and len(res) != numtok:
                    shell.write(f'[FAILED]  Wrong number of tokens, expecting {numtok} got {len(res)}\n', 'COMMENT')
                    nb_tests['failed'] += 1
                    nb_tests['total']  += 1
                    continue
                for assertcheck in asserts:
                    data = assertcheck.split('::')
                    if len(data) == 4:
                        what = data[0]
                        where = data[1]
                        typ = data[2]
                        val = data[3]
                        if what == 'Tokens':
                            where = int(where)
                            if res[where].typ.name != typ:
                                shell.write(f'[FAILED]  Wrong type of token, expecting {typ} got {res[where].typ}\n', 'COMMENT')
                                nb_tests['failed'] += 1
                                nb_tests['total']  += 1
                                continue
                            else:
                                if res[where].val != val: # all value are strings
                                    shell.write(f'[FAILED]  Wrong value of token, expecting {val} got {res[where].val}\n', 'COMMENT')
                                    nb_tests['failed'] += 1
                                    nb_tests['total']  += 1
                                    continue
                                else:
                                    shell.write(f'[ASSERT]  Assert ok for token {where} of type {typ} of val {val}\n', 'STRING')
                # Parsing & Interpreting
                ast = parser.parse(res)
                res = interpreter.do_ast(ast)
                ok = False
                if restyp in type2python:
                    if type(res) == type2python[restyp]:
                        if restyp == 'Boolean':
                            if res == res2python[resval]:
                                ok = True
                        elif restyp == 'Integer':
                            if res == int(resval):
                                ok = True
                        elif restyp == 'String':
                            if "'" + res + "'" == resval:
                                ok = True
                if ok:
                     shell.write(f'[SUCCESS] Expected {resval} of type {restyp} and got: {res} of type {type(res)}\n', 'STRING')
                     nb_tests['success'] += 1
                     nb_tests['total']   += 1
                else:
                    shell.write(f'[FAILED]  Expected {resval} of type {restyp} and got: {res} of type {type(res)}\n', 'COMMENT')
                    nb_tests['failed'] += 1
                    nb_tests['total']  += 1
            except Exception as e:
                shell.write(f'[FAILED]  Exception: {e}\n', 'COMMENT')
                traceback.print_exception(*sys.exc_info())
                nb_tests['failed'] += 1
                nb_tests['total']  += 1
    print('-------------------------------')
    if nb_tests["success"] + nb_tests["failed"] + nb_tests["skipped"] != nb_tests['total']:
        raise Exception("[ERROR] Total of tests not equal to total of tests failed/skipped/success")
    shell.write(f'Nb test success:   {nb_tests["success"]:05d} ({round(nb_tests["success"]/nb_tests["total"]*100):3d}%)\n', 'STRING')
    if nb_tests["failed"] == 0:
        shell.write(f'Nb test failed:    {nb_tests["failed"]:05d} ({round(nb_tests["failed"]/nb_tests["total"]*100):3d}%)\n', 'STRING')
    else:
        shell.write(f'Nb test failed:    {nb_tests["failed"]:05d} ({round(nb_tests["failed"]/nb_tests["total"]*100):3d}%)\n', 'COMMENT')
    shell.write(f'Nb test skipped:   {nb_tests["skipped"]:05d} ({round(nb_tests["skipped"]/nb_tests["total"]*100):3d}%)\n', 'KEYWORD')
    print('-------------------------------')
    print(f'Total test passed: {nb_tests["total"]:05d}')
    print('-------------------------------')
    GLOBAL_DEBUG = False
