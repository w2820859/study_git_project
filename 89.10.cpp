// initial building of enbSCellFreqCandidateList and enbSCellCandidateList
// Get list of neighbour eutran frequencies
if (cellDataPtr == NULL)
{
  return false;
}
U32 pCellId = cellDataPtr->getCellId();
UehCommonCtxtEnbSCellCandidateVectorD sCellCandidateList;
UehCommonCtxtEnbSCellFreqCandidateD sCellFreqCandidate;
UehCommonCtxtEnbSCellFreqCandidateVectorD sCellFreqCandidateList;
bool isDlOnlyFreq = false;
UehCommonCtxtScellFreqPriorityInfoE sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_NONE;
const RnhNCellFreqInfoListHandlerIfD* freqInfoListHandlerPtr =
    freqInfo.getFreqInfoHandlerAccessPtr()->getFreqInfoListHandler();
RnhNCellFreqInfoBaseIfPtrVectorD nFreqInfoList;
RnhNCellFreqInfoFilterD eutranRatFilter;
RnhNCellFreqInfoFilterRuleForRatTypeD ruleForRatType(ELIB_CONST_EUTRANFREQRELATION_FRO_TYPE);
eutranRatFilter.addRule(&ruleForRatType);
freqInfoListHandlerPtr->getFreqInfoList(cellDataPtr->getCellId(), nFreqInfoList, &eutranRatFilter);
 
ELIB_TRACE_DECISION(
    Ft_UE_RAC_MEASUREMENTS,
        "updateEnbSCellCandidateList nFreqInfoList size=%d, pcell cellId = %d",
        nFreqInfoList.size(),
        pCellId);
 
// Get list of neighbour eutran frequencies
for (RnhNCellFreqInfoBaseIfPtrVectorD::const_iterator itFreq = nFreqInfoList.begin();
    itFreq != nFreqInfoList.end();
    ++itFreq)
{ /* for each frequency */     
  
  RnhNCellCellInfoBaseIfPtrVectorD allCellInfos;
  (*itFreq)->getAllCellInfos(allCellInfos);
                     
  // identify the sCellCandidate in each freq
  sCellCandidateList.clear();
 
  for(RnhNCellCellInfoBaseIfPtrVectorD::const_iterator cellInfoIter = allCellInfos.begin();
      cellInfoIter != allCellInfos.end();
      ++cellInfoIter)
  {
   
    if((*cellInfoIter)->getHasRelation(pCellId))
    {
      const RnhNCellEUtranCellGenericInfoIfD* eutranCellPtr = static_cast<const RnhNCellEUtranCellGenericInfoIfD* const>((*cellInfoIter));
 
      ELIB_TRACE_DECISION(
          Ft_UE_RAC_MEASUREMENTS,
              "updateEnbSCellCandidateList scell getFroId  =%d  pecell getFroId =%d, getSCellCandidate =%d",
              (*cellInfoIter)->getCellFroRef().getFroId(),
              cellDataPtr->getFroId(),
              eutranCellPtr->getSCellCandidate(cellDataPtr->getCellId()));
     
      /* for each neighbor cell */
      if(eutranCellPtr->getSCellCandidate(cellDataPtr->getCellId()) == 1)
      {       
        if(cellDataPtr->getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
        {
          // Serving Cell is FDD
          if((*cellInfoIter)->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
          {
            // SCell candidate is FDD, ensure the FroId is different between the PCell and the SCell
            if((*cellInfoIter)->getCellFroRef().getFroId() != cellDataPtr->getFroId())
            {
              // PCell FDD, SCell FDD, add SCell FDD
              saveEnbSCellCandidateInList(eutranCellPtr, sCellCandidateList, isDlOnlyFreq);
              sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_FDD;
             
              ELIB_TRACE_DECISION(
                  Ft_UE_RAC_MEASUREMENTS,
                  "updateEnbSCellCandidateList::PCell FDD, SCell FDD, add SCell FDD, isDlOnlyFreq=%d scell cellId=%d, sCellFreqPriority=%d",
                  isDlOnlyFreq,
                  eutranCellPtr->getCgi().eNodeBCellId.cellId,
                  sCellFreqPriority);
             
            }   
          }
          else if((*cellInfoIter)->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
          {
            // PCell FDD, SCell TDD, add SCell TDD
            // SCell is TDD. don't need to check the FroId.
            saveEnbSCellCandidateInList(eutranCellPtr, sCellCandidateList, isDlOnlyFreq); 
            sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_TDD;
           
            ELIB_TRACE_DECISION(
                Ft_UE_RAC_MEASUREMENTS,
                "updateEnbSCellCandidateList::PCell FDD, SCell TDD, add SCell TDD, isDlOnlyFreq=%d scell cellId=%d, sCellFreqPriority=%d",
                isDlOnlyFreq,
                eutranCellPtr->getCgi().eNodeBCellId.cellId,
                sCellFreqPriority);
           
          } 
        }
        else if(cellDataPtr->getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
        {
          // Serving Cell is TDD
          if((*cellInfoIter)->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
          {
            // SCell candidate is TDD, ensure the FroId is different btween the PCell and the SCell
            if((*cellInfoIter)->getCellFroRef().getFroId() != cellDataPtr->getFroId())
            {
              //PCell TDD, SCell TDD add SCell TDD
              saveEnbSCellCandidateInList(eutranCellPtr, sCellCandidateList, isDlOnlyFreq);
              sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_TDD;
             
              ELIB_TRACE_DECISION(
                  Ft_UE_RAC_MEASUREMENTS,
                  "updateEnbSCellCandidateList::PCell TDD, SCell TDD, add SCell TDD, isDlOnlyFreq=%d scell cellId=%d, sCellFreqPriority=%d",
                  isDlOnlyFreq,
                  eutranCellPtr->getCgi().eNodeBCellId.cellId,
                  sCellFreqPriority);
            }            
          }
          else if((*cellInfoIter)->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
          {
            //PCell TDD, SCell FDD add SCell FDD
            //SCell is FDD. don't need to check the FroId.
            saveEnbSCellCandidateInList(eutranCellPtr, sCellCandidateList, isDlOnlyFreq);
            sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_FDD;
 
            ELIB_TRACE_DECISION(
              Ft_UE_RAC_MEASUREMENTS,
              "updateEnbSCellCandidateList::PCell TDD, SCell FDD, add SCell FDD, isDlOnlyFreq=%d scell cellId=%d, sCellFreqPriority=%d",
              isDlOnlyFreq,
              eutranCellPtr->getCgi().eNodeBCellId.cellId,
              sCellFreqPriority);
          }
        }
      }
    }
  }
  ELIB_TRACE_DECISION(
      Ft_UE_RAC_MEASUREMENTS,
          "updateEnbSCellCandidateList saved the scell, isDlOnlyFreq =%d  sCellCandidateList size =%d, freqId =%d\n",
          isDlOnlyFreq,
          sCellCandidateList.size(),
          (*itFreq)->getFreqId());
 
  if(!sCellCandidateList.empty())
  {
    sCellFreqCandidate.setSCellCandidateList(sCellCandidateList);
    sCellFreqCandidate.freqId = (*itFreq)->getFreqId();
    if(isDlOnlyFreq &&
        sCellFreqPriority == UEH_COMMON_SCELL_FREQ_PRIORITY_FDD)
    {
      // override with FDD DlOnly
      sCellFreqCandidate.sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_FDD_ISDLONLY;
    }
    else
    {
      sCellFreqCandidate.sCellFreqPriority = sCellFreqPriority;
    }
   
    if(sCellFreqCandidate.sCellFreqPriority == UEH_COMMON_SCELL_FREQ_PRIORITY_TDD)
    {
      // TDD freq add to top of list,
      sCellFreqCandidateList.insert(sCellFreqCandidateList.begin(), sCellFreqCandidate);
    }
    else
    {
      // insert the scell freq candidate in the BACK
      sCellFreqCandidateList.push_back(sCellFreqCandidate);
    }
    // reset follow values
    isDlOnlyFreq = false;
    sCellFreqPriority = UEH_COMMON_SCELL_FREQ_PRIORITY_NONE;
  }
 
  ELIB_TRACE_DECISION(
      Ft_UE_RAC_MEASUREMENTS,
          "updateEnbSCellCandidateList sCellFreqCandidateList size=%d sCellFreqPriority=%d \n",
          sCellFreqCandidateList.size(),
          sCellFreqCandidate.sCellFreqPriority);
}
 
//clear the list
cellDataPtr->clearEnbSCellFreqCandidateList();
// save sCellFreqCandidateList in cellData
BT_INFO(STR("updateEnbSCellCandidateList(): sCellFreqCandidateList.size(): %d", sCellFreqCandidateList.size()));
cellDataPtr->setEnbSCellFreqCandidateList(sCellFreqCandidateList);
return true;