#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 
typedef enum {Tnil, Tcons, Tatom} NTag;
typedef enum {Anum, Asym, Astring, Abool} ATag;

typedef struct sexp sexp;
struct sexp {
    NTag tag;
    union {
        struct {sexp *car, *cdr;} cons;
        struct {ATag tag; char *lex;} atom;
    } u; 
};

//from sprint 5:
typedef sexp* env;

static inline sexp *nil(void); //i think this will make it way easier for nil handling might have ot remove or change

static sexp *cons(sexp *a, sexp *b);

static env env_make(void) { //empty environment
    return cons(nil(), cons(nil(), nil()));
}

//from sprint 3:
static sexp *BOOL_TRUE = NULL, *BOOL_FALSE = NULL;//make assigning easier


//EOL points to NIL instead of assigning it
//also make nil easier and might have to change
static sexp NIL_NODE = {.tag = Tnil};
static sexp *NIL = &NIL_NODE;
static inline sexp *nil(void) {return NIL;}
//Construct different types of sexp

static sexp *makeSym(const char *s) {
    sexp *x = (sexp*)malloc(sizeof *x);
    x->tag = Tatom;
    x->u.atom.tag = Asym;
    x->u.atom.lex = strdup(s);
    return x;
    }

static sexp *makeNum(const char *s) {
    sexp *x = (sexp*)malloc(sizeof *x);
    x->tag = Tatom;
    x->u.atom.tag = Anum;
    x->u.atom.lex = strdup(s);
    return x;
}

static sexp *makeString(const char *s) {
    sexp *x = (sexp*)malloc(sizeof *x);
    x->tag = Tatom;
    x->u.atom.tag = Astring;
    x->u.atom.lex = strdup(s);
    return x;
}

sexp *boolTrue(void) {
    if (!BOOL_TRUE) {
        sexp *x = (sexp*)malloc(sizeof *x);
        x->tag = Tatom;
        x->u.atom.tag = Abool;
        x->u.atom.lex = strdup("true");
        BOOL_TRUE = x;
    }
    return BOOL_TRUE;
}

sexp *boolFalse(void) {
    if (!BOOL_FALSE) {
        sexp *x = (sexp*)malloc(sizeof *x);
        x->tag = Tatom;
        x->u.atom.tag = Abool;
        x->u.atom.lex = strdup("false");
        BOOL_FALSE = x;
    }
    return BOOL_FALSE;
}

sexp *makeBool(int v) {return v ? boolTrue() : boolFalse();}


static sexp *cons(sexp *a, sexp *b) {
    sexp *x = (sexp*)malloc(sizeof *x);
    x->tag = Tcons;
    x->u.cons.car = a;
    x->u.cons.cdr = b;
    return x;
}

static sexp *carR(sexp *r) {return (r && r->tag == Tcons) ? r->u.cons.car : NIL;}
static sexp *cdrR(sexp *r) {return (r && r->tag == Tcons) ? r->u.cons.cdr : NIL;}

//print

static void printSexp(sexp *x); //for printList function

static void printAtom(sexp *x) {
    if (x->u.atom.tag == Astring) {
        putchar('"');
        fputs(x->u.atom.lex, stdout);
        putchar('"');
    } else {
        fputs(x->u.atom.lex, stdout);
    }
}
static void printList(sexp *r) {

    putchar ('(');
    if (r == NIL) {putchar (')'); }
    //first
    printSexp(carR(r));
    r = cdrR(r);
    //rest
    while (r != NIL && r->tag == Tcons) {
        putchar (' ');
        printSexp(carR(r));
        r = cdrR(r);
    }

    //end and/or dot notation
    if (r == NIL) {
        putchar(')');
    } else {
        fputs(" . ", stdout);
        printSexp(r);
        putchar(')');
    }
    return;
}
static void printSexp(sexp *x) {
    if (!x || x == NIL) {fputs("()", stdout); return;}
    if (x->tag == Tatom) {
        printAtom(x);
    } else if (x->tag == Tcons) {
        printList(x);
    } else {
        fputs("()", stdout);
    }
}

static int getI(FILE *in) {
    return fgetc(in);
}
static void unGetI(int i, FILE *in) {
    if (i != EOF) {ungetc(i, in); }
}

static void skipWS(FILE *in) {
    int i;
    for (;;) {
        i = getI(in);
        if (i == EOF) {return;}
        if (isspace(i)) continue;
        unGetI(i, in);
        return;
    }
}
//add char
static char *append(char *c, int i) {
    size_t n = c ? strlen(c) : 0;
    char *t = (char*)realloc(c, n + 2);
    t[n] = (char)i;
    t[n+1] = '\0';
    return t;
}
//should I assign in as a num
static int isNumeric(const char *c) {
    int i = 0;
    int dots = 0;
    int digits = 0;
    if (c[i] == '+' || c[i] == '-') i++;
    for (; c[i]; ++i) {
        if (c[i] == '.') {dots++; continue;}
        if (isdigit((unsigned char)c[i])) {digits = 1; continue;}
        return 0;
    }
    return digits && dots <= 1;
}

static sexp *readString(FILE *in) {
    int i;
    char *buf = NULL;
    while ((i = getI(in)) != EOF && i != '"') {
        if (i == '\\') {int e = fgetc(in); if (e == EOF) break; i = e;}
        size_t n = buf ? strlen (buf) : 0;
        char *t = realloc(buf, n + 2);
        if (!t) {free(buf); return makeString("");}
        buf = t;
        buf[n] = (char)i;
        buf[n+1] = '\0';
    }
    sexp *x = makeString(buf ? buf : "");
    free(buf);
    return x;
}

static sexp *symbolOrNumber(FILE *in, int s) {
    int i = s;
    char *buf = NULL;
    buf = append(buf, i);

    while ((i = getI(in)) != EOF) {
        if (isspace(i) || i == '(' || i ==')' || i == '"') {unGetI(i, in); break; }
        buf = append(buf, i);
    }
    sexp *x = isNumeric(buf) ? makeNum(buf) : makeSym(buf);
    free(buf); //potential fix
    return x;
}
//defining for use in readList
static sexp *readSexp(FILE *in);
sexp* atom (const char *s);

static sexp *readList(FILE *in) {
    skipWS(in);
    int i = getI(in);
    if (i == EOF) return NIL;
    if (i == ')') return NIL;
    unGetI(i, in);

    //start of reading list

    sexp *f = readSexp(in);
    sexp *head = cons(f, NIL);
    sexp *tail = head;

    //rest
    for (;;) {
        skipWS(in);
        i = getI(in);
        if (i == EOF) break;
        if (i == ')') break;
        unGetI(i, in);
        sexp *elem = readSexp(in);
        tail->u.cons.cdr = cons(elem, NIL);
        tail = tail->u.cons.cdr;
    }
    return head;
}
static sexp *readSexp(FILE *in) {
    skipWS(in);
    int i = getI(in);
    if (i == EOF) return NULL;
    if (i == '(') return readList(in);
    if (i == ')') return NIL;
    if (i == '"') return readString(in);
    //added after
    if (i == '\'') {
        sexp *x = readSexp(in);
        return cons(atom("quote"), cons(x, nil()));
    }
    return symbolOrNumber(in, i);
}

//clean mem
static void freeSexp(sexp *x) {
    if (!x || x == NIL) return;
    if (x->tag == Tcons) {
        freeSexp(x->u.cons.car);
        freeSexp(x->u.cons.cdr);
        free(x);
    } else if (x->tag == Tatom) {
        free(x->u.atom.lex);
        free(x);
    }
}
//checks
int isNil(const sexp *x) {
    return x && x->tag == Tnil;
}
int isCons(const sexp *x) {
    return x && x->tag == Tcons;
}
int isAtom(const sexp *x) {
    return x && x->tag == Tatom;
}
int isNum(const sexp *x) {
    return x && x->tag == Tatom && x->u.atom.tag == Anum;
}
int isSym(const sexp *x) {
    return x && x->tag == Tatom && x->u.atom.tag == Asym;
}
int isString(const sexp *x) {
    return x && x->tag == Tatom && x->u.atom.tag == Astring;
}
int isList(const sexp *x) {
    return isNil(x) || isCons(x);
}
int isTrueSexp(const sexp *x) {
    return !isNil(x);
}
int isBool(const sexp *x) {
    return x && x->tag == Tatom && x->u.atom.tag == Abool;
}
sexp* atom (const char *s) {
    if (!s) return NIL;
    size_t n = strlen(s);
    if (n>= 2 && s[0] =='"' && s[n-1] == '"') {
        char *inn = strndup(s + 1, n -2);
        sexp *o = makeString(inn ? inn : "");
        free(inn);
        return o;
    }
    return isNumeric(s) ? makeNum(s) : makeSym(s);
}
#define testCase(test, expr) \
    printf("In: %s\n Out: %s\n\n", test, (expr) ? "true" : "false")


//next singular function is from sprint 7 I just want it up here
static int isLambda(sexp *v) {
    if (!v || v->tag != Tcons) return 0;
    sexp *head = carR(v);
    return head && head->tag == Tatom && head->u.atom.tag == Asym && strcmp(head->u.atom.lex, "lambda") == 0;
}
//Start of sprint 3
//right now everything is stored as strings so I need a helper function for the numbers
static int to_i64(const sexp *x, long long *out) {
    if (!x || x->tag != Tatom || x->u.atom.tag != Anum) return 0;
    char *end = NULL;
    long long v = strtoll(x->u.atom.lex, &end, 10);
    if (!(end && *end =='\0')) return 0;
    if (out) *out = v;
    return 1;
}
//Back to sexp
static sexp *from_i64(long long v) {
    char buf[64];
    snprintf(buf, sizeof buf, "%lld", v);
    return makeNum(buf);
}
//error for trying to make a non-number into an atom and other errors
static sexp *err(const char *msg) {
    return cons(atom("error"), atom(msg));
}
//Math functions
sexp *add(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return from_i64(x + y);
}

sexp *sub(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return from_i64(x - y);
}

sexp *mul(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return from_i64(x * y);
}

sexp *divd(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    if (y == 0) return err("trying to divide by zero");
    return from_i64(x / y);
}

sexp *mod(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    if (y == 0) return err("trying to divide by zerro");
    return from_i64(x % y);
}

//equality functions
sexp *lt(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return makeBool(x < y);
}

sexp *gt(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return makeBool(x > y);
}

sexp *lte(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return makeBool(x <= y);
}

sexp *gte(sexp *a, sexp *b) {
    long long x,y;
    if (!to_i64(a,&x) || !to_i64(b,&y)) {
        return err("not a number");
    }
    return makeBool(x >= y);
}
//Equal and Not
sexp *eq(sexp *a, sexp *b) {
    if (a == b) return boolTrue();
    if (!a || !b || a->tag != b->tag) return boolFalse();
    if (a->tag == Tatom) {
        if (a->u.atom.tag != b->u.atom.tag) return boolFalse();
        return strcmp(a->u.atom.lex, b->u.atom.lex) == 0 ? boolTrue() : boolFalse();
    }
    return boolFalse();
}

sexp *not(sexp *x) {
    if (!isBool(x)) {
        return err("not a boolean");
    }
    if (strcmp(x->u.atom.lex, "true") == 0) {
        return boolFalse();
    } else if (strcmp(x->u.atom.lex, "false") == 0) {
        return boolTrue();
    } else {
        return err("Error in not function");
    }
}

//sprint5:
static sexp *env_syms(env e) {return carR(e);}
static sexp *env_vals(env e) {return carR(cdrR(e));}

static void env_setSyms(env e, sexp *new_syms) {e->u.cons.car = new_syms;}
static void env_setVals(env e, sexp *new_vals) {cdrR(e)->u.cons.car = new_vals;}

static void env_bind(env e, sexp *sym, sexp *val) {
    env_setSyms(e, cons(sym, env_syms(e)));
    env_setVals(e, cons(val, env_vals(e)));
}

static int sym_eq(const sexp *a, const sexp *b) {
    return a && b && a->tag == Tatom && a->u.atom.tag == Asym && b->tag == Tatom && b->u.atom.tag == Asym && strcmp(a->u.atom.lex, b->u.atom.lex) == 0;
}

static sexp *env_lookup(env e, sexp *sym) {
    sexp *s = env_syms(e), *v = env_vals(e);
    while (s && s->tag == Tcons && v && v->tag == Tcons) {
        if (sym_eq(carR(s), sym)) return carR(v);
        s = cdrR(s);
        v = cdrR(v);
    }
    return sym;
}

//list function helpers
static int listLen(sexp *x) {
    int n = 0;
    while (x && x->tag == Tcons) {
        n++;
        x = cdrR(x);
    }
    return n;
}
static sexp *nth(sexp *x, int k) {
    while (k-- > 0 && x && x->tag == Tcons) {
        x = cdrR(x);
    }
    return (x && x->tag == Tcons) ? carR(x) : nil();
}
static int isSymNamed(sexp *s, const char *name) {
    return s && s->tag == Tatom && s->u.atom.tag == Asym && strcmp(s->u.atom.lex, name) == 0;
}

//declaration
static sexp *eval(sexp *x, env e);
//static sexp *eval(sexp *x);
static sexp *evalList(sexp *lst, env e) {
    if (lst == nil()) return nil();
    sexp *head = NULL, *tail = NULL;
    while (lst && lst->tag == Tcons) {
        sexp *v = eval(carR(lst), e);
        if (!head) {
            head = cons(v, nil());
            tail = head;
        }
        else {
            tail->u.cons.cdr = cons(v, nil());
            tail = tail->u.cons.cdr; //keep going
        }
        lst = cdrR(lst);
    }
    return head ? head : nil(); //checks to see
}

static sexp *built_in_sym(sexp *op, sexp *arg) {
    if (!op || op->tag != Tatom || op->u.atom.tag != Asym) return NULL;
    const char *f = op->u.atom.lex;
    int n = listLen(arg);

    if (!strcmp(f, "nil?")) {
        if (n != 1) {return cons(atom("error"), atom("arity"));}
        return makeBool(isNil(nth(arg, 0)));
    }

    if (!strcmp(f, "symbol?")) {
        if (n != 1) {return cons(atom("error"), atom("arity"));}
        sexp *x = nth(arg, 0);
        return makeBool(x && x->tag == Tatom && x->u.atom.tag == Asym);
    }

    if (!strcmp(f, "number?")) {
        if (n != 1) {return cons(atom("error"), atom("arity"));}
        sexp *x = nth(arg, 0);
        return makeBool(x && x->tag == Tatom && x->u.atom.tag == Anum);
    }

    if (!strcmp(f, "string?")) {
        if (n != 1) {return cons(atom("error"), atom("arity"));}
        sexp *x = nth(arg, 0);
        return makeBool(x && x->tag == Tatom && x->u.atom.tag == Astring);
    }

    if (!strcmp(f, "list?")) {
        if (n != 1) {return cons(atom("error"), atom("arity"));}
        sexp *x = nth(arg, 0);
        return makeBool(x && (x->tag == Tcons || x->tag == Tnil));
    }

    if (!strcmp(f,"cons")) { 
        if(n!=2) return cons(atom("error"),atom("arity"));
        return cons(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f,"car")) { 
        if(n!=1) return cons(atom("error"),atom("arity"));
        sexp *s = nth(arg, 0);
        return (s && s->tag == Tcons) ? carR(s) : nil();
    }
    if (!strcmp(f,"cdr")) { 
        if(n!=1) return cons(atom("error"),atom("arity"));
        sexp *s = nth(arg, 0);
        return (s && s->tag == Tcons) ? cdrR(s) : nil();
    }
    if (!strcmp(f, "cdrr")) {
        sexp *s = nth(arg, 0);
        return (s &&s->tag) ? cdrR(cdrR(s)) : nil();
    }

    if (!strcmp(f, "add") || !strcmp(f, "+")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return add(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "sub") || !strcmp(f, "-")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return sub(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "mul") || !strcmp(f, "*")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return mul(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "div") || !strcmp(f, "/")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return divd(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "mod") || !strcmp(f, "%")) { // this might cause problems the percent sign is lit up red.
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return mod(nth(arg, 0), nth(arg, 1));
    }

    if (!strcmp(f, "lt") || !strcmp(f, "<")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return lt(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "gt") || !strcmp(f, ">")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return gt(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "lte") || !strcmp(f, "<=")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return lte(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "gte") || !strcmp(f, ">=")) {
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return gte(nth(arg, 0), nth(arg, 1));
    }

    if (!strcmp(f, "eq") || !strcmp(f, "=")) {//not sure if i can keep the equal sign. IDK if it'll mess it up marked for potential fix
        if(n != 2) return cons(atom("error"), atom ("arity"));
        return eq(nth(arg, 0), nth(arg, 1));
    }
    if (!strcmp(f, "not") || !strcmp(f, "!")) {
        if(n != 1) return cons(atom("error"), atom ("arity"));
        return not(nth(arg, 0));
    }
    /* (cadr x) => (car (cdr x)) */
    /*if (!strcmp(f, "cadr")) {
        if (n != 1) return err("arity");
        sexp *x = carR(arg);          // first argument
        if (!x || x->tag != Tcons) return err("need list");
        return carR(cdrR(x));
    }

    /* (caddr x) => (car (cdr (cdr x))) */
    /*if (!strcmp(f, "caddr")) {
        if (n != 1) return err("arity");
        sexp *x = carR(arg);          // first argument
        if (!x || x->tag != Tcons) return err("need list");
        sexp *t = cdrR(x);
        if (!t || t->tag != Tcons) return err("short list");
        t = cdrR(t);
        if (!t || t->tag != Tcons) return err("short list");
        return cßarR(t);
    }*/
   /* (nth xs n)  -> 0-based index */
    if (!strcmp(f, "nth")) {
        //if (n != 2) return err("arity");       // need (nth list index)
        sexp *xs = nth(arg, 0);                // args are already evaluated
        sexp *ni = nth(arg, 1);
        long long i;
        if (!isNum(ni)) return err("index not number");
        to_i64(ni, &i);

        // adapt to your number accessor
        if (i < 0) return err("negative index");

        while (i > 0) {
            if (!xs || xs->tag != Tcons) return err("short list");
            xs = cdrR(xs);
            --i;
        }
        if (!xs || xs->tag != Tcons) return err("short list");
        return carR(xs);
    }

    return NULL;
}
static sexp *userLambda(sexp *l, sexp *arg, env e) {//so users can add lambda funcs
    sexp *form = carR(cdrR(l));
    sexp *body = carR(cdrR(cdrR(l)));

    //eval
    sexp *act = evalList(arg, e);

    if (listLen(form) != listLen(act)) {return cons(atom("error"), atom("arity"));}

    sexp *oldS = env_syms(e);
    sexp *oldV = env_vals(e);

    //potential mem fix
    sexp *f = form, *a = act;
    while (f && f->tag == Tcons && a && a->tag == Tcons) {
        sexp *x = carR(f);
        sexp *y = carR(a);
        env_setSyms(e, cons(x, env_syms(e)));
        env_setVals(e, cons(y, env_vals(e)));
        f = cdrR(f);
        a = cdrR(a);
    }

    sexp *ret = eval(body, e);

    env_setSyms(e, oldS);
    env_setVals(e, oldV);
    
    return ret;
}
//attempt to fix factorial problem
static int isTrue(const sexp *x) {
    if (x && x->tag == Tatom && x->u.atom.tag == Abool) {
        return x == BOOL_TRUE;
    }
    return !isNil((sexp*)x);
}
static sexp *eval(sexp *x, env e) {
    if (!x) return nil();
    if (x->tag == Tnil) return nil();

    if (x->tag == Tatom) {
        if (x->u.atom.tag == Asym) return env_lookup(e, x);
        return x;
    }

    sexp *op = carR(x);
    sexp *arg = cdrR(x);

    if (isSymNamed(op, "quote")) {
        return carR(arg);
    }
    if (isSymNamed(op, "set")) {
        sexp *name = carR(arg);
        sexp *val = eval (carR(cdrR(arg)), e);
        env_bind(e, name, val);
        return val;
    }

    //from sprint 6:
    //these used isNil in the if statement before added isTrue function to see if it would fix my test case testing for factorial.
    //if other things break change back to isNil
    if (isSymNamed(op, "and")) {
        if (listLen(arg) != 2) return err("arity");
        sexp *arg1 = eval(carR(arg), e);
        if (!isTrue(arg1)) return nil();
        return eval(carR(cdrR(arg)), e);
    }

    if (isSymNamed(op, "or")) {
        if (listLen(arg) != 2) return err("arity");
        sexp *e1 = eval(carR(arg), e);
        if (isTrue(e1)) return boolTrue();
        return eval(carR(cdrR(arg)), e);
    }

    if (isSymNamed(op, "if")) {
        if (listLen(arg) != 3) return err("arrity");
        sexp *tmp = eval(carR(arg), e);
        if (isTrue(tmp)) return eval(carR(cdrR(arg)), e);
        return eval(carR(cdrR(cdrR(arg))), e);
    }

    if (isSymNamed(op, "cond")) {
        sexp *cls = arg;
        while(cls && cls->tag == Tcons) {
            sexp *pr = carR(cls);
            if (!(pr && pr->tag == Tcons && cdrR(pr) && cdrR(pr)->tag == Tcons)) {
                return err("bad cond clause");
            }
            sexp *tmp1 = carR(pr);
            sexp *tmp2 = carR(cdrR(pr));
            sexp *tmp3 = eval(tmp1, e);
            if (isTrue(tmp3)) return eval(tmp2, e);
            cls = cdrR(cls);
        }
        return nil();
    }

    if (isSymNamed(op, "define")) {
        sexp *n = carR(arg);
        sexp *form = carR(cdrR(arg));
        sexp *b = carR(cdrR(cdrR(arg)));

        sexp *fun = cons(atom("lambda"), cons(form, cons(b, nil())));
        env_bind(e, n, fun);

        return n;
    }

    if (isSymNamed(op, "lambda")) {
        return x;//don't eval yet
    }

    if (op && op->tag == Tatom && op->u.atom.tag == Asym) {
        sexp *opVal = env_lookup(e, op);
        if (isLambda(opVal)) {
            return userLambda(opVal, arg, e);
        }
    }
    sexp *call = eval(op, e);//lets user functions over ride built in ones
    if (isLambda(call)) {
        return userLambda(call, arg, e);
    }

    sexp *evalArg = evalList(arg, e);
    sexp *poss = built_in_sym(op, evalArg);
    if (poss) return poss;
    return cons(atom("error"), atom("unknown error"));
    
}

static env REPL_ENV = NULL;

static void repl(void) {
    if (!REPL_ENV) REPL_ENV = env_make();
    for(;;) {
        fputs("lisp> ", stdout);
        fflush(stdout);
        sexp *in = readSexp(stdin);
        if (!in) {fputc('\n', stdout); break;}
        sexp *out = eval(in, REPL_ENV);
        printSexp(out);
        fputc('\n', stdout);

    }
}
static void runTests(void) {
    env T = env_make();

    printf("START OF SPRINT 1 and 2 TESTS: \n\n"); 

    sexp *tst = cons(atom("a"), cons(atom("b"), nil()));
    
    printf("IN: car((a b))\n Out ");
    printSexp(carR(tst));
    printf("\n\n"); //should be "a"

    printf("In: cdr((a b))\n Out: ");
    printSexp(cdrR(tst));
    printf("\n\n"); //should be "b"

    testCase("isNil((a b))", isNil(tst)); //false

    testCase("isNil(nil)", isNil(nil()));//true
    testCase("isNil(\"x\")", isNil(atom("x"))); //false
    testCase("isSymbol(atom(\"x\"))", isSym(atom("x")));//true
    testCase("isList(nil())", isList(nil()));
    testCase("isTrueSexp(nil())", isTrueSexp(nil()));//true
    testCase("isTrueSexp(cons(atom(\"a\"), nil()))", isTrueSexp(cons(atom("a"), nil())));//ttrue
    testCase("isString(atom(\"\"hi\"\"))", isString(atom("\"hi\""))); //true
    testCase("isNumber(atom(\"-3.14\"))", isNum(atom("-3.14"))); //true

    //sprint 3 tests
    printf("START OF SPRINT 3 TESTS: \n\n");
    sexp *r;

    r = add(atom("1"), atom("2"));
    printf("In: add(1,2)\n Out: ");
    printSexp(r); //3
    printf("\n\n");
    freeSexp(r);

    r = sub(atom("5"), atom("2"));
    printf("In: sub(5,2)\n Out: ");
    printSexp(r); //3
    printf("\n\n");
    freeSexp(r);

    r = mul(atom("3"), atom("4"));
    printf("In: mul(3,4)\n Out: ");
    printSexp(r); //12
    printf("\n\n");
    freeSexp(r);

    r = divd(atom("9"), atom("3"));
    printf("In: divd(9,3)\n Out: ");
    printSexp(r); //3
    printf("\n\n");
    freeSexp(r);

    r = mod(atom("9"), atom("4"));
    printf("In: mod(9,4)\n Out: ");
    printSexp(r); //1
    printf("\n\n");
    freeSexp(r);

    r = divd(atom("9"), atom("0"));
    printf("In: divd(9,0)\n Out: ");
    printSexp(r); //error divide by zero
    printf("\n\n");
    freeSexp(r);

    printf("IN: lt(2,3)\n Out: ");
    printSexp(lt(atom("2"), atom ("3")));
    printf("\n\n");

    printf("IN: gt(5,1)\n Out: ");
    printSexp(gt(atom("5"), atom ("1")));
    printf("\n\n");

    printf("IN: lte(5,5)\n Out: ");
    printSexp(lte(atom("5"), atom ("5")));
    printf("\n\n");

    printf("IN: gte(6,5)\n Out: ");
    printSexp(gte(atom("6"), atom ("5")));
    printf("\n\n");

    printf("IN: lt(3,2)\n Out: ");
    printSexp(lt(atom("3"), atom ("2")));
    printf("\n\n");


    printf("IN: eq(a,a)\n Out: ");
    printSexp(eq(atom("a"), atom ("a")));
    printf("\n\n");

    printf("IN: not(true)\n Out: ");
    printSexp(not(boolTrue()));
    printf("\n\n");

    printf("IN: not(false)\n Out: ");
    printSexp(not(boolFalse()));
    printf("\n\n");

    //sprint 5 tests.
    printf("START OF SPRINT 5 TESTS: \n\n"); 

    sexp *l123   = cons(atom("1"), cons(atom("2"), cons(atom("3"), nil())));
    sexp *q_l123 = cons(atom("quote"), cons(l123, nil()));
    printf("In: (quote (1 2 3))\n Out: ");
    printSexp(eval(q_l123, T)); printf("\n\n");                       //123

    sexp *q_add = cons(atom("quote"), cons(cons(atom("add"), cons(atom("1"), cons(atom("2"), nil()))), nil()));
    printf("In: (quote (add 1 2))\n Out: ");
    printSexp(eval(q_add, T)); printf("\n\n");                         //(add 1 2)

    // setting vals
    sexp *set_x_5 = cons(atom("set"), cons(atom("x"), cons(atom("5"), nil())));
    printf("In: (set x 5)\n Out: ");
    printSexp(eval(set_x_5, T)); printf("\n\n");                       //5

    printf("In: x\n Out: ");
    printSexp(eval(atom("x"), T)); printf("\n\n");                     //5

    printf("In: y\n Out: ");
    printSexp(eval(atom("y"), T)); printf("\n\n");                     //y (unbound)

    //test symbol use
    sexp *plus12 = cons(atom("+"), cons(atom("1"), cons(atom("2"), nil())));
    printf("In: (+ 1 2)\n Out: ");
    printSexp(eval(plus12, T)); printf("\n\n"); // 3

    sexp *div93 = cons(atom("/"), cons(atom("9"), cons(atom("3"), nil())));
    printf("In: (/ 9 3)\n Out: ");
    printSexp(eval(div93, T)); printf("\n\n"); // 3

    // trying to test different order for the eval func
    eval(cons(atom("set"), cons(atom("x"), cons(atom("0"), nil()))), T);        
    sexp *set_x_1  = cons(atom("set"), cons(atom("x"), cons(atom("1"), nil())));
    sexp *add_expr = cons(atom("add"), cons(set_x_1, cons(atom("x"), nil())));
    printf("In: (add (set x 1) x)\n Out: ");
    printSexp(eval(add_expr, T)); printf("\n\n");                      //2

    eval(cons(atom("set"), cons(atom("x"), cons(atom("2"), nil()))), T);       
    sexp *set_x_10   = cons(atom("set"), cons(atom("x"), cons(atom("10"), nil())));
    sexp *add_1_set  = cons(atom("add"), cons(atom("1"), cons(set_x_10, nil())));
    sexp *mul_expr   = cons(atom("mul"), cons(add_1_set, cons(atom("x"), nil())));
    printf("In: (mul (add 1 (set x 10)) x)\n Out: ");
    printSexp(eval(mul_expr, T)); printf("\n\n");                      //110

    //testing different predicates with eval fun
    sexp *ab       = cons(atom("a"), cons(atom("b"), nil()));
    sexp *q_ab     = cons(atom("quote"), cons(ab, nil()));
    sexp *car_ab   = cons(atom("car"), cons(q_ab, nil()));
    sexp *cdr_ab   = cons(atom("cdr"), cons(q_ab, nil()));
    sexp *l23      = cons(atom("2"), cons(atom("3"), nil()));
    sexp *q_23     = cons(atom("quote"), cons(l23, nil()));
    sexp *cons_1_23= cons(atom("cons"), cons(atom("1"), cons(q_23, nil())));

    printf("In: (car (quote (a b)))\n Out: ");
    printSexp(eval(car_ab, T)); printf("\n\n");                        //a

    printf("In: (cdr (quote (a b)))\n Out: ");
    printSexp(eval(cdr_ab, T)); printf("\n\n");                        //(b)

    printf("In: (cons 1 (quote (2 3)))\n Out: ");
    printSexp(eval(cons_1_23, T)); printf("\n\n");                     //(1 2 3)

    sexp *q_symx = cons(atom("quote"), cons(atom("x"), nil()));
    printf("In: (symbol? (quote x))\n Out: ");
    printSexp(eval(cons(atom("symbol?"), cons(q_symx, nil())), T)); printf("\n\n"); //true

    printf("In: (symbol? x)\n Out: ");
    printSexp(eval(cons(atom("symbol?"), cons(atom("x"), nil())), T)); printf("\n\n"); // false

    //bools
    sexp *add_12 = cons(atom("add"), cons(atom("1"), cons(atom("2"), nil())));
    sexp *eq1    = cons(atom("eq"), cons(atom("3"), cons(add_12, nil())));
    printf("In: (eq 3 (add 1 2))\n Out: ");
    printSexp(eval(eq1, T)); printf("\n\n");                           //true

    sexp *qa1 = cons(atom("quote"), cons(atom("a"), nil()));
    sexp *qa2 = cons(atom("quote"), cons(atom("a"), nil()));
    sexp *eq2 = cons(atom("eq"), cons(qa1, cons(qa2, nil())));
    printf("In: (eq (quote a) (quote a))\n Out: ");
    printSexp(eval(eq2, T)); printf("\n\n");                           //true

    sexp *ab1 = cons(atom("a"), cons(atom("b"), nil()));
    sexp *ab2 = cons(atom("a"), cons(atom("b"), nil()));
    sexp *ql1 = cons(atom("quote"), cons(ab1, nil()));
    sexp *ql2 = cons(atom("quote"), cons(ab2, nil()));
    sexp *eq3 = cons(atom("eq"), cons(ql1, cons(ql2, nil())));
    printf("In: (eq (quote (a b)) (quote (a b)))\n Out: ");
    printSexp(eval(eq3, T)); printf("\n\n");                           //false

    // using not with bools
    sexp *lt12   = cons(atom("lt"), cons(atom("1"), cons(atom("2"), nil())));
    sexp *not_lt = cons(atom("not"), cons(lt12, nil()));
    printf("In: (not (lt 1 2))\n Out: ");
    printSexp(eval(not_lt, T)); printf("\n\n");                        //false

    sexp *gte55 = cons(atom("gte"), cons(atom("5"), cons(atom("5"), nil())));
    printf("In: (gte 5 5)\n Out: ");
    printSexp(eval(gte55, T)); printf("\n\n");                         //true

    //error
    sexp *foo = cons(atom("foo"), cons(atom("1"), cons(atom("2"), nil())));
    printf("In: (foo 1 2)\n Out: ");
    printSexp(eval(foo, T)); printf("\n\n");

    //Sprint 6 tests:
    printf("START OF SPRINT 6 TESTS: \n\n"); 
    
    eval(cons(atom("set"), cons(atom("x"), cons(atom("0"), nil()))), T);
    sexp *and1 = cons(atom("and"), cons(nil(), cons(cons(atom("set"), cons(atom("x"), cons(atom("1"), nil()))), nil())));
    printf("In: (and () (set x 1))\n Out: "); printSexp(eval(and1, T)); printf("\n");
    printf("In: x\n Out: "); printSexp(eval(atom("x"), T)); printf("\n\n"); //0

    sexp *and2 = cons(atom("and"), cons(atom("5"), cons(atom("7"), nil())));
    printf("In: (and 5 7)\n Out: "); printSexp(eval(and2, T)); printf("\n\n"); //7

    // or
    eval(cons(atom("set"), cons(atom("x"), cons(atom("0"), nil()))), T);
    sexp *or1 = cons(atom("or"), cons(atom("42"), cons(cons(atom("set"), cons(atom("x"), cons(atom("2"), nil()))), nil())));
    printf("In: (or 42 (set x 2))\n Out: "); printSexp(eval(or1, T)); printf("\n");
    printf("In: x\n Out: "); printSexp(eval(atom("x"), T)); printf("\n\n"); //0

    sexp *or2 = cons(atom("or"), cons(nil(), cons(atom("9"), nil())));
    printf("In: (or () 9)\n Out: "); printSexp(eval(or2, T)); printf("\n\n"); //9

    //if
    sexp *if1 = cons(atom("if"), cons(nil(), cons(atom("1"), cons(atom("2"), nil()))));
    printf("In: (if () 1 2)\n Out: "); printSexp(eval(if1, T)); printf("\n\n"); //2
    sexp *if2 = cons(atom("if"), cons(atom("3"), cons(atom("1"), cons(atom("2"), nil()))));
    printf("In: (if 3 1 2)\n Out: "); printSexp(eval(if2, T)); printf("\n\n"); //1

    //cond
    sexp *cond1 = cons(atom("cond"), cons(cons(nil(), cons(atom("1"), nil())), cons(cons(atom("3"), cons(atom("99"), nil())), nil())));
    printf("In: (cond (() 1) (3 99))\n Out: "); printSexp(eval(cond1, T)); printf("\n\n"); //99

    sexp *cond2 = cons(atom("cond"), cons(cons(nil(), cons(atom("1"), nil())), cons(cons(nil(), cons(atom("2"), nil())), nil())));
    printf("In: (cond (() 1) (() 2))\n Out: "); printSexp(eval(cond2, T)); printf("\n\n"); // ()

    //sprint 7 tests

    printf("START OF SPRINT 7 TESTS: \n\n");
    sexp *def_inc = cons(atom("define"), cons(atom("inc"), cons(cons(atom("x"), nil()), cons(cons(atom("+"), cons(atom("x"), cons(atom("1"), nil()))), nil()))));
    printf("In: (define inc (x) (+ x 1))\n Out: ");
    printSexp(eval(def_inc, T)); printf("\n\n");               // inc

    sexp *call_inc = cons(atom("inc"), cons(atom("5"), nil()));
    printf("In: (inc 5)\n Out: ");
    printSexp(eval(call_inc, T)); printf("\n\n");                    //6

    /* arity errors */
    printf("In: (inc)\n Out: ");
    printSexp(eval(cons(atom("inc"), nil()), T)); printf("\n\n");    //error arity)
    printf("In: (inc 1 2)\n Out: ");
    printSexp(eval(cons(atom("inc"), cons(atom("1"), cons(atom("2"), nil()))), T));
    printf("\n\n");                                                  //(error arity)

    eval(cons(atom("set"), cons(atom("a"), cons(atom("10"), nil()))), T);
    sexp *def_usex = cons(atom("define"), cons(atom("usex"), cons(cons(atom("x"), nil()), cons(cons(atom("+"), cons(atom("x"), cons(atom("a"), nil()))), nil()))));
    eval(def_usex, T);
    printf("In: (usex 5)\n Out: ");
    printSexp(eval(cons(atom("usex"), cons(atom("5"), nil())), T)); printf("\n\n"); //15

    // recursion
    sexp *def_fact = cons(atom("define"), cons(atom("fact"), cons(cons(atom("n"), nil()), cons(cons(atom("if"), cons(cons(atom("="), cons(atom("n"), cons(atom("0"), nil()))), cons(atom("1"), cons(cons(atom("*"), cons(atom("n"), cons(cons(atom("fact"), cons(cons(atom("-"), cons(atom("n"), cons(atom("1"), nil()))), nil())), nil()))), nil())))), nil()))));
    eval(def_fact, T);
    printf("In: (fact 5)\n Out: ");
    printSexp(eval(cons(atom("fact"), cons(atom("5"), nil())), T)); printf("\n\n"); //120

    // override 
    sexp *def_plus = cons(atom("define"), cons(atom("+"), cons(cons(atom("a"), cons(atom("b"), nil())), cons(atom("0"), nil()))));
    eval(def_plus, T);

    printf("In: (+ 2 3)\n Out: ");
    printSexp(eval(cons(atom("+"), cons(atom("2"), cons(atom("3"), nil()))), T));
    printf("\n\n");  //0 

    //sprint 8 tests
    env env8 = env_make(); //new environment for test 8
    //had issues with old test cases overriding this one.
    //override piece from previous sprint test
    printf("START OF SPRINT 8 TESTS:\n\n");

    sexp *lam_call = cons( cons(atom("lambda"), cons( cons(atom("x"), nil()), cons( cons(atom("+"), cons(atom("x"), cons(atom("1"), nil()))), nil()))), cons(atom("5"), nil()));
    printf("In: ((lambda (x) (+ x 1)) 5)\n Out: ");
    printSexp(eval(lam_call, env8)); printf("\n\n");  //6

    sexp *def_square = cons(atom("define"), cons(atom("square"), cons(cons(atom("x"), nil()), cons(cons(atom("*"), cons(atom("x"), cons(atom("x"), nil()))), nil()))));  //square
    printf("In: (define square (x) (* x x))\n Out: ");
    printSexp(eval(def_square, env8)); printf("\n\n");

    printf("In: (square 5)\n Out: ");
    printSexp(eval(cons(atom("square"), cons(atom("5"), nil())), env8)); printf("\n\n");       //25

    (void)T; // get rid of environments
    (void)env8;
}
int main(void) {
    /*while(1) {
        sexp *x = readSexp(stdin);
        if (!x) break;
        printSexp(x);
        putchar('\n');
        freeSexp(x);
    }
    return 0;*/
    //These are test cases. the expected value is commented next to each test case

    char buf[32];

    fputs("Select mode:\n", stdout);
    fputs(" 1) Run test cases\n", stdout);
    fputs(" 2) User mode (REPL)\n", stdout);
    fputs("Choice: ", stdout);
    fflush(stdout);

    if (!fgets(buf, sizeof buf, stdin)) {fputc('\n', stdout); return 0;}

    char *p = buf;
    while (*p == ' ' || *p == '\t') ++p;

    if (*p == '1') {
        runTests();
        return 0;
    } else if(*p == '2') {
        REPL_ENV = env_make();
        repl();
        return 0;
    } else {
        fputs("Invalid choice. Exiting.\n", stdout);
        return 0;
    }

    
    return 0;
}