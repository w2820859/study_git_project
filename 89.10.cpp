  if(testSuite == "UehUeSCellEvalBasicTestSuiteD")
  {
    return new UehUeSCellEvalBasicTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalTwoScellSelectionPolicyTestSuiteD")
  {
    return new UehUeSCellEvalTwoScellSelectionPolicyTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalBasicScellSelectionPolicyTestSuiteD")
  {
    return new UehUeSCellEvalBasicScellSelectionPolicyTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalScellSelectionPolicyTDDTestSuiteD")
  {
    return new UehUeSCellEvalScellSelectionPolicyTDDTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalDynamicSingleScellSelectionPolicyTestSuiteD")
  {
    return new UehUeSCellEvalDynamicSingleScellSelectionPolicyTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalScellSelectionPolicyFDDTDDMixTestSuiteD")
  {
    return new UehUeSCellEvalScellSelectionPolicyFDDTDDMixTestSuiteD;
 }
  else if (testSuite == "UehUeSCellEvalScellSelectionWithReducedMeasurementPolicyTestSuiteD")
 
  {
    return new UehUeSCellEvalScellSelectionWithReducedMeasurementPolicyTestSuiteD;
  }
  else if (testSuite == "UehUeSCellEvalScellSelectionPolicyTDDFDDMixTestSuiteD")
  {
    return new UehUeSCellEvalScellSelectionPolicyTDDFDDMixTestSuiteD;
  }
 
 
  TRACE_ERROR(STR("Test suite %s doesn't exist in testSuite().", (char*)testSuite));
  return NULL;