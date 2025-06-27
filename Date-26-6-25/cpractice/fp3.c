#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TreeNode {
    char *val;
    int (*evaluate)(struct TreeNode *);
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

int eval(TreeNode *node) {
    if (!node->left && !node->right)
        return atoi(node->val);

    int left = node->left->evaluate(node->left);
    int right = node->right->evaluate(node->right);

    if (strcmp(node->val, "+") == 0) return left + right;
    if (strcmp(node->val, "-") == 0) return left - right;
    if (strcmp(node->val, "*") == 0) return left * right;
    if (strcmp(node->val, "/") == 0) return left / right;

    return 0;
}

TreeNode *newNode(char *val) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->val = strdup(val);
    node->left = node->right = NULL;
    node->evaluate = eval;
    return node;
}

int isOperator(char *s) {
    return (strcmp(s, "+") == 0 || strcmp(s, "-") == 0 ||
            strcmp(s, "*") == 0 || strcmp(s, "/") == 0);
}

TreeNode *buildTree(char **postfix, int size) {
    TreeNode *stack[100];
    int top = -1;

    for (int i = 0; i < size; i++) {
        TreeNode *node = newNode(postfix[i]);

        if (isOperator(postfix[i])) {
            node->right = stack[top--];
            node->left = stack[top--];
        }

        stack[++top] = node;
    }

    return stack[top];
}

int main() {
    char *postfix[] = {"3", "4", "+", "2", "*", "7", "/"};
    int size = sizeof(postfix) / sizeof(postfix[0]);

    TreeNode *exprTree = buildTree(postfix, size);
    int result = exprTree->evaluate(exprTree);
    printf("Result: %d\n", result);  

    return 0;
}

