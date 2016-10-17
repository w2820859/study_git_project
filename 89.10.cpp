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
else if (testSuite == "UehUeSCellEvalUplinkScellSelectionPolicyTestSuiteD")
{
  return new UehUeSCellEvalUplinkScellSelectionPolicyTestSuiteD;
}
else if (testSuite == "UehUeSCellEvalThreeScellSelectionPolicyTestSuiteD")
{
  return new UehUeSCellEvalThreeScellSelectionPolicyTestSuiteD;
}
else if (testSuite == "UehUeSCellEvalFourScellSelectionPolicyTestSuiteD")
{
  return new UehUeSCellEvalFourScellSelectionPolicyTestSuiteD;
}
else if (testSuite == "UehUeSCellEvalDynamicSingleScellSelectionWithReducedMeasurementPolicyTestSuiteD")
{
  return new UehUeSCellEvalDynamicSingleScellSelectionWithReducedMeasurementPolicyTestSuiteD;
}
else if (testSuite == "UehUeSCellEvalTwoScellSelectionPolicyTDDTestSuiteD")
{
  return new UehUeSCellEvalTwoScellSelectionPolicyTDDTestSuiteD;
}
 
TRACE_ERROR(STR("Test suite %s doesn't exist in testSuite().", (char*)testSuite));
return NULL;