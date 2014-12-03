from __future__ import division
import numpy as np
from sklearn.cross_validation import StratifiedKFold
from sklearn.ensemble import RandomForestClassifier, ExtraTreesClassifier
from sklearn.linear_model import LogisticRegression

def read_data(file_name):
    f = open(file_name,'r')
    f=f.readlines()
    f=f[1:]	
    samples = []
    target = []
    for line in f:
        sample_ = line.strip().split(',')

	for x in range(len(sample_)):
		if sample_[x]=='':
		       sample_.remove(sample_[x])

	sample = [float(x) for x in sample_]
	if len(sample_) == 1777:
	        samples.append(sample)
    return samples

def load():
    print "Loading data..."
    train = read_data("data/train1.csv")
    test = read_data("data/test1.csv")
    
    y_train = np.array([x[0] for x in train])
    X_train = np.array([x for x in train])
   
    print "Train Complete"

#   y_test = np.array([x[0] for x in test])
    X_test = np.array([x[1:] for x in test])
    return X_train, y_train, X_test




def logloss(attempt, actual, epsilon=1.0e-15):
    """Logloss, i.e. the score of the bioresponse competition.
    """
    attempt = np.clip(attempt, epsilon, 1.0-epsilon)
    return - np.mean(actual * np.log(attempt) + (1.0 - actual) * np.log(1.0 - attempt))



def classifier():
    np.random.seed(0) # seed to shuffle the train set

    n_folds = 3
    verbose = True
    shuffle = False

    X, y, X_submission = load()

    if shuffle:
        idx = np.random.permutation(y.size)
        X = X[idx]
        y = y[idx]

    skf = list(StratifiedKFold(y, n_folds))

    clfs = [RandomForestClassifier(n_estimators=10, n_jobs=-1, criterion='gini')]
	#	 RandomForestClassifier(n_estimators=200, n_jobs=-1, criterion='entropy'),
	 #   ExtraTreesClassifier(n_estimators=200, n_jobs=-1, criterion='gini'),
	  #  ExtraTreesClassifier(n_estimators=200, n_jobs=-1, criterion='entropy')]
           # GradientBoostingClassifier(learn_rate=0.05, subsample=0.5, max_depth=6, n_estimators=50)]

    print "Creating train and test sets for blending."
    
    dataset_blend_train = np.zeros((X.shape[0], len(clfs)))
    dataset_blend_test = np.zeros((X_submission.shape[0], len(clfs)))
    
    for j, clf in enumerate(clfs):
        print j, clf
        dataset_blend_test_j = np.zeros((X_submission.shape[0], len(skf)))
        for i, (train, test) in enumerate(skf):
            print "Fold", i
            X_train = X[train]
            y_train = y[train]
            X_test = X[test]
            y_test = y[test]
          
#	    for item in X_train:
#		if len(item)!=1776:
#			print len(item)
	    
	    clf.fit(X_train, y_train)
	   
            y_submission = clf.predict_proba(X_test)[:,1]
            dataset_blend_train[test, j] = y_submission
            dataset_blend_test_j[:, i] = clf.predict_proba(X_submission)[:,1]
	    dataset_blend_test[:,j] = dataset_blend_test_j.mean(1)
	
    print "Blending."
    clf = LogisticRegression()
    clf.fit(dataset_blend_train, y)
    y_submission = clf.predict_proba(dataset_blend_test)[:,1]



    print "Linear stretch of predictions to [0,1]"
    y_submission = (y_submission - y_submission.min()) / (y_submission.max() - y_submission.min())

    print "Saving Results."
    np.savetxt(fname='test_ans.csv', X=y_submission, fmt='%0.9f')
    
#    print "LogLoss."
#    print logloss(y_submission,soln)

if __name__ == '__main__':
	classifier()
