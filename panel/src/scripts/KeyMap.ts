export const KEYMAP = new Map<string, string>();
export const REVERSE_KEYMAP = new Map<string, string>();
KEYMAP.set('0', "Keyboard.ZERO");
KEYMAP.set('1', "Keyboard.ONE");
KEYMAP.set('2', "Keyboard.TWO");
KEYMAP.set('3', "Keyboard.THREE");
KEYMAP.set('4', "Keyboard.FOUR");
KEYMAP.set('5', "Keyboard.FIVE");
KEYMAP.set('6', "Keyboard.SIX");
KEYMAP.set('7', "Keyboard.SEVEN");
KEYMAP.set('8', "Keyboard.EIGHT");
KEYMAP.set('9', "Keyboard.NINE");
let A = 'A'.charCodeAt(0);
let Z = 'Z'.charCodeAt(0);
for (let i = A; i <= Z; i++) {
    let c = String.fromCharCode(i);
    KEYMAP.set(c, `Keyboard.${c}`);
}
KEYMAP.forEach((v, k) => {
    REVERSE_KEYMAP.set(v, k);
});

