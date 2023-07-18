from datetime import datetime
import itertools
import json

import matplotlib.pyplot as plt
import numpy as np
from sklearn.metrics import confusion_matrix
from sklearn.tree import _tree


# https://stackoverflow.com/questions/50916422/python-typeerror-object-of-type-int64-is-not-json-serializable
class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        if isinstance(obj, np.floating):
            return float(obj)
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super(NpEncoder, self).default(obj)


def current_time():
    time_str = str(datetime.now()).replace(' ', '_').replace(':', '-')
    return time_str[:time_str.find('.')]


def plot_error_cdf(Y_true, Y_pred,
                   save_path=None, figsize=(6, 6)):
    fig, ax = plt.subplots(figsize=figsize)
    ax.axvline(x=0, color='red', linewidth=2, linestyle='--')
    plot_cdf(Y_pred - Y_true, ax=ax)
    ax.set_xlabel('Area Error (pixel)', fontsize=20, fontweight='bold')
    ax.set_ylabel('Probability', fontsize=20, fontweight='bold')
    ax.set_xlim(-60000, 60000)
    decorate_cdf(ax)

    if save_path is not None:
        plt.savefig(save_path)
    return fig


def plot_cdf(value, save_path=None, ax=None, figsize=(6, 6)):
    if ax is None:
        fig, ax = plt.subplots(figsize=figsize)
    else:
        fig = None
    ax.plot(np.sort(value), np.linspace(0, 1, len(value)), linewidth=3, color='blue')
    ax.set_xlim(np.min(value), np.max(value))
    decorate_cdf(ax)
    if save_path is not None:
        plt.savefig(save_path)
    return fig


def decorate_cdf(ax):
    ax.set_ylim(0, 1)
    labels = ax.get_xticklabels() + ax.get_yticklabels()
    [label.set_fontsize(12) for label in labels]
    [label.set_fontweight('bold') for label in labels]



# Ref https://www.kaggle.com/grfiv4/plot-a-confusion-matrix
def plot_confusion_matrix(y_true, y_pred,
                          save_path=None,
                          target_names=None,
                          normalize=False,
                          figsize=(12, 10),
                          title='Confusion matrix',
                          cmap=plt.get_cmap('Blues'),
                          fontsize=30,
                          vmax=None):
    '''
    given a sklearn confusion matrix (cm), make a nice plot

    Arguments
    ---------
    cm:           confusion matrix from sklearn.metrics.confusion_matrix

    target_names: given classification classes such as [0, 1, 2]
                  the class names, for example: ['high', 'medium', 'low']

    title:        the text to display at the top of the matrix

    cmap:         the gradient of the values displayed from matplotlib.pyplot.cm
                  see http://matplotlib.org/examples/color/colormaps_reference.html
                  plt.get_cmap('jet') or plt.cm.Blues

    normalize:    If False, plot the raw numbers
                  If True, plot the proportions

    Usage
    -----
    plot_confusion_matrix(cm           = cm,                  # confusion matrix created by
                                                              # sklearn.metrics.confusion_matrix
                          normalize    = True,                # show proportions
                          target_names = y_labels_vals,       # list of names of the classes
                          title        = best_estimator_name) # title of graph

    Citiation
    ---------
    http://scikit-learn.org/stable/auto_examples/model_selection/plot_confusion_matrix.html

    '''

    # Compute confusion matrix
    cm = confusion_matrix(y_true, y_pred)

    if normalize:
        cm = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]

    fig, ax = plt.subplots(figsize=figsize)
    im = ax.imshow(cm, interpolation='nearest', cmap=cmap,
                   vmin=0 if normalize else None,
                   vmax=1 if normalize else (None if vmax is None else vmax))
    cb = fig.colorbar(im, fraction=0.046, pad=0.04)
    cb.ax.tick_params(labelsize=fontsize)
    # ax.set_title(title, fontsize=fontsize)

    if target_names is None:
        ax.get_xaxis().set_ticks([])
        ax.get_yaxis().set_ticks([])
    else:
        tick_marks = np.arange(len(target_names))
        ax.set_xticks(tick_marks)
        ax.set_yticks(tick_marks)
        ax.set_xticklabels(target_names, fontsize=fontsize)
        ax.set_yticklabels(target_names, fontsize=fontsize)

    thresh = np.mean(cm) if vmax is None else vmax / 2
    for i, j in itertools.product(range(cm.shape[0]), range(cm.shape[1])):
        if normalize:
            ax.text(j, i, "{:0.2f}".format(cm[i, j]),
                    horizontalalignment="center", fontsize=fontsize,
                    color="white" if cm[i, j] > thresh else "black")
        else:
            ax.text(j, i, "{:,}".format(cm[i, j]),
                    horizontalalignment="center", fontsize=fontsize,
                    color="white" if cm[i, j] > thresh else "black")

    ax.set_ylabel('True label', fontsize=fontsize)
    ax.set_xlabel('Predicted label', fontsize=fontsize)
    fig.tight_layout()
    if save_path is not None:
        plt.savefig(save_path)
    return fig


def tree_to_code_cpp(tree, feature_names, indent=2, name='scaler'):
    indent = ' ' * indent
    tree_ = tree.tree_
    feature_name = [
        feature_names[i] if i != _tree.TREE_UNDEFINED else 'undefined!'
        for i in tree_.feature
    ]
    feature_name_with_type = [
        f'float {f}' for f in feature_names
    ]
    code = ''
    code += f'float {name}({", ".join(feature_name_with_type)}) {{\n'

    def recurse(node, depth):
        code = ''
        if tree_.feature[node] != _tree.TREE_UNDEFINED:
            name = feature_name[node]
            threshold = tree_.threshold[node]
            code += indent * depth + f'if ({name} <= {threshold:.5g}) {{\n'
            code += recurse(tree_.children_left[node], depth + 1)
            code += indent * depth + \
                f'else {{  // if {name} > {threshold:.5g}\n'
            code += recurse(tree_.children_right[node], depth + 1)
        else:
            code += indent * depth + \
                f'return {np.argmax(tree_.value[node][0]):.5g};\n'
        code += indent * (depth - 1) + '}\n'
        return code

    code += recurse(0, 1)
    return code
